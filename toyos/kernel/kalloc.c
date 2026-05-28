#include "kernel/defs.h"
#include "kernel/memlayout.h"

extern char end[];

struct run {
  struct run *next;
};

static struct {
  struct run *freelist;
} kmem;

void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (uint64)pa < (uint64)end || (uint64)pa >= PHYSTOP) {
    printf("kfree: invalid page %p\n", pa);
    for(;;)
      ;
  }

  memset(pa, 1, PGSIZE);
  r = (struct run *)pa;
  r->next = kmem.freelist;
  kmem.freelist = r;
}

void *
kalloc(void)
{
  struct run *r = kmem.freelist;

  if(r)
    kmem.freelist = r->next;

  if(r)
    memset((char *)r, 5, PGSIZE);

  return (void *)r;
}

void
kinit(void)
{
  char *p = (char *)PGROUNDUP((uint64)end);

  for(; p < (char *)TRAPFRAME_BASE; p += PGSIZE) {
    if((uint64)p >= USER_BASE && (uint64)p < USER_STACK)
      continue;
    kfree(p);
  }
}
