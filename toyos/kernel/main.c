#include "kernel/defs.h"

void
main(void)
{
  printf("Hello World, ToyOS is initializing.\n");
  printf("ToyOS printf works: %d %x %s\n", 123, 0xabc, "ok");

  for(;;)
    ;
}