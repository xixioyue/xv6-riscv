#include "kernel/defs.h"
#include "kernel/memlayout.h"
#include "kernel/riscv.h"
#include "kernel/vm.h"

pagetable_t kernel_pagetable;

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

  if(mappages(kpgtbl, KERNBASE, USER_BASE - KERNBASE, KERNBASE, PTE_R | PTE_W | PTE_X) != 0) {
    printf("kvmmake: kernel direct map before user failed\n");
    for(;;)
      ;
  }

  if(mappages(kpgtbl, USER_BASE, USER_STACK - USER_BASE, USER_BASE, PTE_R | PTE_W | PTE_X | PTE_U) != 0) {
    printf("kvmmake: user direct map failed\n");
    for(;;)
      ;
  }

  if(mappages(kpgtbl, USER_STACK, PHYSTOP - USER_STACK, USER_STACK, PTE_R | PTE_W | PTE_X) != 0) {
    printf("kvmmake: kernel direct map after user failed\n");
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
