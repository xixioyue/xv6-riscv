#include "kernel/defs.h"
#include "kernel/memlayout.h"
#include "kernel/riscv.h"

struct trapframe *tf = (struct trapframe *)TRAPFRAME_BASE;

extern void uservec(void);
extern void userret(void);
extern void switch_to_user(void);

extern char _binary_user_initcode_bin_start[];
extern char _binary_user_initcode_bin_end[];

void
trap_init(void)
{
  w_stvec((uint64)uservec);
  tf->kernel_trap = (uint64)usertrap;
}

void
load_user_program(void)
{
  uint64 size = _binary_user_initcode_bin_end - _binary_user_initcode_bin_start;

  printf("loading user initcode to %p, size=%d bytes\n", USER_BASE, size);
  memmove((void *)USER_BASE, _binary_user_initcode_bin_start, size);
}

void
enter_user_space(void)
{
  switch_to_user();
}

void
usertrap(void)
{
  if((r_sstatus() & SSTATUS_SPP) != 0) {
    printf("panic: usertrap not from user mode\n");
    for(;;)
      ;
  }

  tf->epc = r_sepc();

  if(r_scause() == 8) {
    tf->epc += 4;
    intr_on();
    syscall();
  } else {
    printf("unexpected trap scause=%p sepc=%p\n", r_scause(), r_sepc());
    for(;;)
      ;
  }

  intr_off();

  uint64 x = r_sstatus();
  x &= ~SSTATUS_SPP;
  x |= SSTATUS_SPIE;
  w_sstatus(x);
  w_sepc(tf->epc);

  userret();
}