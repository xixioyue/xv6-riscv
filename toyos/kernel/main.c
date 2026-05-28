#include "kernel/defs.h"

void
main(void)
{
  printf("Hello World, ToyOS is initializing.\n");
  printf("ToyOS printf works: %d %x %s\n", 123, 0xabc, "ok");

  kinit();
  void *page = kalloc();
  printf("physical page allocator initialized, first page=%p\n", page);
  if(page)
    kfree(page);

  pagetable_t test_pagetable = (pagetable_t)kalloc();
  memset(test_pagetable, 0, PGSIZE);
  if(mappages(test_pagetable, 0x0, PGSIZE, USER_BASE, PTE_R | PTE_X | PTE_U) == 0)
    printf("page table test: va 0x0 -> pa %p\n", USER_BASE);

  trap_init();
  load_user_program();
  printf("entering user space...\n");
  enter_user_space();

  for(;;)
    ;
}
