#include "kernel/apps.h"
#include "kernel/defs.h"
#include "kernel/elf.h"
#include "kernel/memlayout.h"
#include "kernel/riscv.h"

struct proc proc[NPROC];
static int nextpid = 1;
static struct proc *curproc;
static struct proc *initproc;
static struct context scheduler_context;

extern char _binary_user_initcode_bin_start[];
extern char _binary_user_initcode_bin_end[];

static void
safestrcpy(char *dst, const char *src, uint n)
{
  if(n == 0)
    return;

  while(--n > 0 && *src)
    *dst++ = *src++;

  *dst = 0;
}

static struct proc *
allocproc(void)
{
  for(struct proc *p = proc; p < &proc[NPROC]; p++) {
    if(p->state != UNUSED)
      continue;

    memset(p, 0, sizeof(*p));
    p->state = USED;
    p->pid = nextpid++;

    p->trapframe = (struct trapframe *)kalloc();
    p->kstack = (uint64)kalloc();
    if(p->trapframe == 0 || p->kstack == 0) {
      if(p->trapframe)
        kfree(p->trapframe);
      if(p->kstack)
        kfree((void *)p->kstack);
      p->state = UNUSED;
      return 0;
    }

    memset(p->trapframe, 0, PGSIZE);
    memset((void *)p->kstack, 0, PGSIZE);
    return p;
  }

  return 0;
}

static void
setup_trapframe(struct proc *p, uint64 epc, uint64 sp)
{
  p->trapframe->kernel_satp = MAKE_SATP(kernel_pagetable);
  p->trapframe->kernel_sp = p->kstack + PGSIZE;
  p->trapframe->kernel_trap = (uint64)usertrap;
  p->trapframe->kernel_hartid = r_tp();
  p->trapframe->epc = epc;
  p->trapframe->sp = sp;
}

static void
setup_context(struct proc *p)
{
  memset(&p->context, 0, sizeof(p->context));
  p->context.ra = (uint64)usertrapret;
  p->context.sp = p->kstack + PGSIZE;
}

pagetable_t
proc_pagetable(struct proc *p)
{
  pagetable_t pagetable = kvmmake();
  if(pagetable == 0)
    return 0;

  if(mappages(pagetable, TRAPFRAME_BASE, PGSIZE, (uint64)p->trapframe, PTE_R | PTE_W) != 0) {
    proc_freepagetable(pagetable, 0);
    return 0;
  }

  return pagetable;
}

void
procinit(void)
{
  memset(proc, 0, sizeof(proc));
  appsinit();
  curproc = 0;
  initproc = 0;
  nextpid = 1;
}

struct proc *
current_proc(void)
{
  return curproc;
}

void
set_current_proc(struct proc *p)
{
  curproc = p;
}

void
userinit(void)
{
  struct proc *p = allocproc();
  if(p == 0) {
    printf("userinit: allocproc failed\n");
    for(;;)
      ;
  }

  p->pagetable = proc_pagetable(p);
  if(p->pagetable == 0) {
    printf("userinit: proc_pagetable failed\n");
    for(;;)
      ;
  }

  uint64 init_size = _binary_user_initcode_bin_end - _binary_user_initcode_bin_start;
  uint64 sz = uvmalloc(p->pagetable, 0, USER_INIT_STACK, PTE_R | PTE_W | PTE_X | PTE_U);
  if(sz == 0) {
    printf("userinit: uvmalloc failed\n");
    for(;;)
      ;
  }

  uint64 pa = walkaddr(p->pagetable, USER_ENTRY);
  if(pa == 0 || init_size > PGSIZE) {
    printf("userinit: bad initcode mapping\n");
    for(;;)
      ;
  }

  memmove((void *)pa, _binary_user_initcode_bin_start, init_size);

  p->sz = sz;
  setup_trapframe(p, USER_ENTRY, USER_INIT_STACK);
  setup_context(p);
  safestrcpy(p->name, "initcode", sizeof(p->name));
  p->state = RUNNABLE;
  initproc = p;

  printf("created init process pid=%d, initcode size=%d bytes\n", p->pid, init_size);
}

void
freeproc(struct proc *p)
{
  if(p->pagetable)
    proc_freepagetable(p->pagetable, p->sz);
  if(p->trapframe)
    kfree(p->trapframe);
  if(p->kstack)
    kfree((void *)p->kstack);

  memset(p, 0, sizeof(*p));
  p->state = UNUSED;
}

// 父进程得到子进程 PID；子进程的 trapframe->a0 被设为 0，
int
fork_proc(void)
{
  struct proc *parent = current_proc();
  struct proc *child = allocproc();
  if(child == 0)
    return -1;

  child->pagetable = proc_pagetable(child);
  if(child->pagetable == 0) {
    freeproc(child);
    return -1;
  }

  if(uvmcopy(parent->pagetable, child->pagetable, parent->sz) < 0) {
    freeproc(child);
    return -1;
  }

  child->sz = parent->sz;
  memmove(child->trapframe, parent->trapframe, sizeof(*child->trapframe));
  child->trapframe->a0 = 0;
  setup_trapframe(child, child->trapframe->epc, child->trapframe->sp);
  setup_context(child);

  child->parent = parent;
  safestrcpy(child->name, parent->name, sizeof(child->name));
  child->state = RUNNABLE;

  printf("fork: parent pid=%d child pid=%d\n", parent->pid, child->pid);
  return child->pid;
}

int
exec_proc(int app_id)
{
  struct proc *p = current_proc();
  pagetable_t pagetable;
  pagetable_t oldpagetable;
  uint64 oldsz;
  uint64 entry = 0;
  uint64 sz = 0;

  if(app_id < 0 || (uint64)app_id >= apps_count)
    return -1;

  struct app_entry *ae = &apps_table[app_id];
  if(ae->start == 0 || ae->size == 0)
    return -1;

  pagetable = proc_pagetable(p);
  if(pagetable == 0)
    return -1;

  if(elf_load_from_mem(pagetable, ae->start, ae->size, &entry, &sz) < 0)
    goto bad;

  sz = PGROUNDUP(sz);
  uint64 newsz = uvmalloc(pagetable, sz, sz + (USERSTACK + 1) * PGSIZE, PTE_R | PTE_W | PTE_U);
  if(newsz == 0)
    goto bad;

  sz = newsz;
  uvmclear(pagetable, sz - (USERSTACK + 1) * PGSIZE);

  oldpagetable = p->pagetable;
  oldsz = p->sz;

  p->pagetable = pagetable;
  p->sz = sz;
  setup_trapframe(p, entry, sz);
  safestrcpy(p->name, ae->name, sizeof(p->name));

  proc_freepagetable(oldpagetable, oldsz);
  printf("exec: pid=%d app=%s entry=%p size=%d\n", p->pid, ae->name, entry, sz);
  return 0;

bad:
  proc_freepagetable(pagetable, sz);
  return -1;
}

void
sched(void)
{
  struct proc *p = current_proc();
  swtch(&p->context, &scheduler_context);
}

void
scheduler(void)
{
  for(;;) {
    intr_on();
    int found = 0;

    for(struct proc *p = proc; p < &proc[NPROC]; p++) {
      if(p->state != RUNNABLE)
        continue;

      found = 1;
      p->state = RUNNING;
      curproc = p;
      swtch(&scheduler_context, &p->context);
      curproc = 0;
    }

    if(!found) {
      printf("no runnable process, system halted\n");
      sbi_shutdown();
    }
  }
}

void
wakeup(void *chan)
{
  if(chan == 0)
    return;

  for(struct proc *p = proc; p < &proc[NPROC]; p++) {
    if(p->state == SLEEPING && p->chan == chan)
      p->state = RUNNABLE;
  }
}

int
wait_proc(uint64 addr)
{
  struct proc *p = current_proc();

  for(;;) {
    int havekids = 0;

    for(struct proc *pp = proc; pp < &proc[NPROC]; pp++) {
      if(pp->parent != p)
        continue;

      havekids = 1;
      if(pp->state == ZOMBIE) {
        int pid = pp->pid;
        if(addr != 0 && copyout(p->pagetable, addr, (char *)&pp->xstate, sizeof(pp->xstate)) < 0)
          return -1;
        freeproc(pp);
        return pid;
      }
    }

    if(!havekids || p->killed)
      return -1;

    p->chan = p;
    p->state = SLEEPING;
    sched();
    p->chan = 0;
  }
}

void
exit_proc(int status)
{
  struct proc *p = current_proc();

  printf("exit: pid=%d status=%d\n", p->pid, status);

  for(struct proc *pp = proc; pp < &proc[NPROC]; pp++) {
    if(pp->parent == p) {
      pp->parent = initproc;
      wakeup(initproc);
    }
  }

  wakeup(p->parent);

  p->xstate = status;
  p->state = ZOMBIE;
  sched();

  printf("exit: zombie process returned from sched\n");
  for(;;)
    ;
}
