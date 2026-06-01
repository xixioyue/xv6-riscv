#include "kernel/defs.h"

void
console_putc(int c)
{
  sbi_call(1, c, 0, 0);
}
