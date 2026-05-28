#include "kernel/defs.h"
#include "kernel/syscall.h"

extern struct trapframe *tf;

static uint64
sys_print(void)
{
  const char *s = (const char *)tf->a0;

  while(*s)
    console_putc(*s++);

  return 0;
}

void
syscall(void)
{
  int num = tf->a7;

  switch(num) {
  case SYS_print:
    tf->a0 = sys_print();
    break;
  default:
    printf("unknown syscall %d\n", num);
    tf->a0 = -1;
    break;
  }
}