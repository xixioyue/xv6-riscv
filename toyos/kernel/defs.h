#include "kernel/types.h"
#include "kernel/memlayout.h"
#include "kernel/vm.h"
#include "kernel/proc.h"


extern pagetable_t kernel_pagetable;
extern struct proc proc[NPROC];

int sbi_call(uint64 which, uint64 arg0, uint64 arg1, uint64 arg2);
void sbi_shutdown(void);
void console_putc(int c);
void printf(const char *fmt, ...);

void start(void);
void main(void);

void *memmove(void *dst, const void *src, uint n);
void *memset(void *dst, int c, uint n);
void kinit(void);
void *kalloc(void);
void kfree(void *pa);
pte_t *walk(pagetable_t pagetable, uint64 va, int alloc);
uint64 walkaddr(pagetable_t pagetable, uint64 va);
int mappages(pagetable_t pagetable, uint64 va, uint64 size, uint64 pa, int perm);
pagetable_t kvmmake(void);
void kvminit(void);
void kvminithart(void);
uint64 uvmalloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz, int perm);
void uvmunmap(pagetable_t pagetable, uint64 va, uint64 npages, int do_free);
void proc_freepagetable(pagetable_t pagetable, uint64 sz);
int uvmcopy(pagetable_t old, pagetable_t new, uint64 sz);
void uvmclear(pagetable_t pagetable, uint64 va);
int copyin(pagetable_t pagetable, char *dst, uint64 srcva, uint64 len);
int copyout(pagetable_t pagetable, uint64 dstva, char *src, uint64 len);
int copyinstr(pagetable_t pagetable, char *dst, uint64 srcva, uint64 max);
uint64 kernel_satp(void);
uint64 user_satp(void);
void vmprint(pagetable_t);
int elf_load_from_mem(pagetable_t pagetable, const void *buf, uint64 bufsz,
                      uint64 *entry_out, uint64 *out_sz);

void appsinit(void);
void procinit(void);
struct proc *current_proc(void);
void set_current_proc(struct proc *p);
pagetable_t proc_pagetable(struct proc *p);
void userinit(void);
void freeproc(struct proc *p);
int fork_proc(void);
int exec_proc(int app_id);
int wait_proc(uint64 addr);
void exit_proc(int status);
void scheduler(void);
void sched(void);
void wakeup(void *chan);
void swtch(struct context *old, struct context *new);
void trap_init(void);
void enter_user_space(void);
void usertrapret(void);
void usertrap(void);
void syscall(void);
