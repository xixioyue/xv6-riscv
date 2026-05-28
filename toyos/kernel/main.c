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

  kvminit();
  kvminithart();
  printf("kernel paging enabled with direct map\n");
  vmprint(kernel_pagetable);

  procinit();
  userinit();
  trap_init();
  printf("entering first process...\n");
  enter_user_space();

  for(;;)
    ;
}
