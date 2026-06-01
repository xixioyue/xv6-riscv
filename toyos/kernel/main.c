#include "kernel/defs.h"
#include "kernel/memlayout.h"
#include "kernel/riscv.h"
#include "kernel/vm.h"
#include "kernel/types.h"

void
main(void)
{
  printf("Hello World, ToyOS is initializing.\n");
  printf("ToyOS printf works: %d %x %s\n", 123, 0xabc, "ok");
  // 内核内存、页表初始化
  kinit();
  void *page = kalloc();
  printf("physical page allocator initialized, first page=%p\n\n", page);
  if(page)
    kfree(page);

  kvminit();
  kvminithart();
  printf("kernel paging enabled with direct map\n");
  vmprint(kernel_pagetable);
  printf("kernel page table initialized\n\n");

  procinit();
  userinit();
  trap_init();
  printf("starting scheduler...\n");
  scheduler();

  for(;;)
    ;
}
