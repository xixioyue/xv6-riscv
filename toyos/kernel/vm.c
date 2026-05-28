#include "kernel/defs.h"
#include "kernel/memlayout.h"
#include "kernel/riscv.h"
#include "kernel/vm.h"

pagetable_t kernel_pagetable;
pagetable_t user_pagetable;

#define VMPRINT_MAX_LEAVES 32

pte_t *
walk(pagetable_t pagetable, uint64 va, int alloc)
{
  if(va >= MAXVA) {
    printf("walk: va too large %p\n", va);
    for(;;)
      ;
  }

  for(int level = 2; level > 0; level--) {
    pte_t *pte = &pagetable[PX(level, va)];

    if(*pte & PTE_V) {
      pagetable = (pagetable_t)PTE2PA(*pte);
    } else {
      if(!alloc)
        return 0;

      pagetable_t newpt = (pagetable_t)kalloc();
      if(newpt == 0)
        return 0;

      memset(newpt, 0, PGSIZE);
      *pte = PA2PTE(newpt) | PTE_V;
      pagetable = newpt;
    }
  }

  return &pagetable[PX(0, va)];
}

int
mappages(pagetable_t pagetable, uint64 va, uint64 size, uint64 pa, int perm)
{
  uint64 a;
  uint64 last;
  pte_t *pte;

  if((va % PGSIZE) != 0) {
    printf("mappages: va not aligned %p\n", va);
    return -1;
  }

  if((size % PGSIZE) != 0 || size == 0) {
    printf("mappages: bad size %d\n", size);
    return -1;
  }

  a = va;
  last = va + size - PGSIZE;

  for(;;) {
    pte = walk(pagetable, a, 1);
    if(pte == 0)
      return -1;

    if(*pte & PTE_V) {
      printf("mappages: remap va=%p\n", a);
      return -1;
    }

    *pte = PA2PTE(pa) | perm | PTE_V;

    if(a == last)
      break;

    a += PGSIZE;
    pa += PGSIZE;
  }

  return 0;
}

pagetable_t
kvmmake(void)
{
  pagetable_t kpgtbl = (pagetable_t)kalloc();
  if(kpgtbl == 0) {
    printf("kvmmake: kalloc failed\n");
    for(;;)
      ;
  }

  memset(kpgtbl, 0, PGSIZE);

  if(mappages(kpgtbl, KERNBASE, PHYSTOP - KERNBASE, KERNBASE, PTE_R | PTE_W | PTE_X) != 0) {
    printf("kvmmake: kernel direct map failed\n");
    for(;;)
      ;
  }

  return kpgtbl;
}

void
kvminit(void)
{
  kernel_pagetable = kvmmake();
}

void
kvminithart(void)
{
  sfence_vma();
  w_satp(MAKE_SATP(kernel_pagetable));
  sfence_vma();
  w_sstatus(r_sstatus() | SSTATUS_SUM);
}

pagetable_t
uvmmake(void)
{
  pagetable_t upgtbl = (pagetable_t)kalloc();
  if(upgtbl == 0) {
    printf("uvmmake: kalloc failed\n");
    for(;;)
      ;
  }

  memset(upgtbl, 0, PGSIZE);

  if(mappages(upgtbl, KERNBASE, USER_BASE - KERNBASE, KERNBASE, PTE_R | PTE_W | PTE_X) != 0) {
    printf("uvmmake: kernel map before user failed\n");
    for(;;)
      ;
  }

  if(mappages(upgtbl, USER_BASE, USER_STACK - USER_BASE, USER_BASE, PTE_R | PTE_W | PTE_X | PTE_U) != 0) {
    printf("uvmmake: user map failed\n");
    for(;;)
      ;
  }

  if(mappages(upgtbl, USER_STACK, PHYSTOP - USER_STACK, USER_STACK, PTE_R | PTE_W | PTE_X) != 0) {
    printf("uvmmake: kernel map after user failed\n");
    for(;;)
      ;
  }

  return upgtbl;
}

void
uvminit(void)
{
  user_pagetable = uvmmake();
}

uint64
kernel_satp(void)
{
  return MAKE_SATP(kernel_pagetable);
}

uint64
user_satp(void)
{
  struct proc *p = current_proc();
  return MAKE_SATP(p->pagetable);
}

uint64
walkaddr(pagetable_t pagetable, uint64 va)
{
  pte_t *pte;

  if(va >= MAXVA)
    return 0;

  pte = walk(pagetable, va, 0);
  if(pte == 0)
    return 0;
  if((*pte & PTE_V) == 0)
    return 0;
  if((*pte & PTE_U) == 0)
    return 0;

  return PTE2PA(*pte);
}

uint64
uvmalloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz, int perm)
{
  char *mem;
  uint64 a;

  if(newsz < oldsz)
    return oldsz;

  oldsz = PGROUNDUP(oldsz);
  for(a = oldsz; a < newsz; a += PGSIZE) {
    mem = kalloc();
    if(mem == 0)
      return 0;

    memset(mem, 0, PGSIZE);
    if(mappages(pagetable, a, PGSIZE, (uint64)mem, perm) != 0) {
      kfree(mem);
      return 0;
    }
  }

  return newsz;
}

void
uvmunmap(pagetable_t pagetable, uint64 va, uint64 npages, int do_free)
{
  uint64 a;

  if((va % PGSIZE) != 0)
    return;

  for(a = va; a < va + npages * PGSIZE; a += PGSIZE) {
    pte_t *pte = walk(pagetable, a, 0);
    if(pte == 0 || (*pte & PTE_V) == 0)
      continue;

    if((*pte & (PTE_R | PTE_W | PTE_X)) == 0)
      continue;

    if(do_free) {
      uint64 pa = PTE2PA(*pte);
      kfree((void *)pa);
    }

    *pte = 0;
  }
}

static void
freewalk(pagetable_t pagetable)
{
  for(int i = 0; i < 512; i++) {
    pte_t pte = pagetable[i];
    if((pte & PTE_V) == 0)
      continue;

    if((pte & (PTE_R | PTE_W | PTE_X)) == 0) {
      freewalk((pagetable_t)PTE2PA(pte));
    }

    pagetable[i] = 0;
  }

  kfree((void *)pagetable);
}

void
proc_freepagetable(pagetable_t pagetable, uint64 sz)
{
  if(pagetable == 0)
    return;

  uvmunmap(pagetable, TRAPFRAME_BASE, 1, 0);
  if(sz > 0)
    uvmunmap(pagetable, 0, PGROUNDUP(sz) / PGSIZE, 1);

  freewalk(pagetable);
}

int
uvmcopy(pagetable_t old, pagetable_t new, uint64 sz)
{
  for(uint64 i = 0; i < sz; i += PGSIZE) {
    pte_t *pte = walk(old, i, 0);
    if(pte == 0 || (*pte & PTE_V) == 0 || (*pte & PTE_U) == 0)
      continue;

    uint64 pa = PTE2PA(*pte);
    int flags = PTE_FLAGS(*pte);
    char *mem = kalloc();
    if(mem == 0)
      return -1;

    memmove(mem, (char *)pa, PGSIZE);
    if(mappages(new, i, PGSIZE, (uint64)mem, flags) != 0) {
      kfree(mem);
      return -1;
    }
  }

  return 0;
}

void
uvmclear(pagetable_t pagetable, uint64 va)
{
  pte_t *pte = walk(pagetable, va, 0);
  if(pte)
    *pte &= ~PTE_U;
}

int
copyin(pagetable_t pagetable, char *dst, uint64 srcva, uint64 len)
{
  while(len > 0) {
    uint64 va0 = PGROUNDDOWN(srcva);
    uint64 pa0 = walkaddr(pagetable, va0);
    if(pa0 == 0)
      return -1;

    uint64 n = PGSIZE - (srcva - va0);
    if(n > len)
      n = len;

    memmove(dst, (void *)(pa0 + (srcva - va0)), n);

    len -= n;
    dst += n;
    srcva = va0 + PGSIZE;
  }

  return 0;
}

int
copyout(pagetable_t pagetable, uint64 dstva, char *src, uint64 len)
{
  while(len > 0) {
    uint64 va0 = PGROUNDDOWN(dstva);
    pte_t *pte = walk(pagetable, va0, 0);
    if(va0 >= MAXVA || pte == 0 || (*pte & PTE_V) == 0 ||
       (*pte & PTE_U) == 0 || (*pte & PTE_W) == 0)
      return -1;

    uint64 pa0 = PTE2PA(*pte);
    uint64 n = PGSIZE - (dstva - va0);
    if(n > len)
      n = len;

    memmove((void *)(pa0 + (dstva - va0)), src, n);

    len -= n;
    src += n;
    dstva = va0 + PGSIZE;
  }

  return 0;
}

int
copyinstr(pagetable_t pagetable, char *dst, uint64 srcva, uint64 max)
{
  for(uint64 i = 0; i < max; i++) {
    if(copyin(pagetable, &dst[i], srcva + i, 1) < 0)
      return -1;
    if(dst[i] == '\0')
      return 0;
  }

  return -1;
}

static void
vmprint_indent(int depth)
{
  for(int i = 0; i < depth; i++)
    printf(".. ");
}

static void
pgtblprint(pagetable_t pagetable, int depth, int *leaf_count, int *omitted)
{
  for(int i = 0; i < 512; i++) {
    pte_t pte = pagetable[i];
    if((pte & PTE_V) == 0)
      continue;

    uint64 pa = PTE2PA(pte);
    int is_leaf = (pte & (PTE_R | PTE_W | PTE_X)) != 0;

    if(is_leaf) {
      if(*leaf_count < VMPRINT_MAX_LEAVES) {
        vmprint_indent(depth);
        printf("%d: pte %p pa %p flags %p\n", i, pte, pa, (uint64)(pte & 0x3ff));
      } else {
        (*omitted)++;
      }
      (*leaf_count)++;
    } else {
      vmprint_indent(depth);
      printf("%d: pte %p next %p\n", i, pte, pa);
      pgtblprint((pagetable_t)pa, depth + 1, leaf_count, omitted);
    }
  }
}

void
vmprint(pagetable_t pagetable)
{
  int leaf_count = 0;
  int omitted = 0;

  printf("page table %p\n", pagetable);
  pgtblprint(pagetable, 1, &leaf_count, &omitted);

  if(omitted > 0)
    printf("... %d leaf mappings omitted\n", omitted);

  printf("page table leaf mappings: %d\n", leaf_count);
}
