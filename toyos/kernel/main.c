#include "kernel/defs.h"

void
main(void)
{
  printf("Hello World, ToyOS is initializing.\n");
  printf("ToyOS printf works: %d %x %s\n", 123, 0xabc, "ok");

  trap_init();
  load_user_program();
  printf("entering user space...\n");
  enter_user_space();

  for(;;)
    ;
}