#include "kernel/defs.h"
#include "kernel/memlayout.h"
#include "kernel/riscv.h"

extern void uservec(void);
extern void userret(void);

void
trap_init(void)
{
  w_stvec((uint64)uservec);
}

void
usertrapret(void)
{
  struct proc *p = current_proc();

  intr_off();
  w_stvec((uint64)uservec);

  uint64 x = r_sstatus();
  x &= ~SSTATUS_SPP;
  x |= SSTATUS_SPIE;
  w_sstatus(x);
  w_sepc(p->trapframe->epc);

  userret();
}

void
enter_user_space(void)
{
  usertrapret();
}

void
usertrap(void)
{
  struct proc *p = current_proc();
  struct trapframe *tf = p->trapframe;

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

  usertrapret();
}
