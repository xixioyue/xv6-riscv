#include "kernel/types.h"
#include "kernel/vm.h"

#define NPROC 8

enum procstate {
  UNUSED,
  USED,
  SLEEPING,
  RUNNABLE,
  RUNNING,
  ZOMBIE,
};

struct context {
  uint64 ra;
  uint64 sp;
  uint64 s0;
  uint64 s1;
  uint64 s2;
  uint64 s3;
  uint64 s4;
  uint64 s5;
  uint64 s6;
  uint64 s7;
  uint64 s8;
  uint64 s9;
  uint64 s10;
  uint64 s11;
};

// Saved user registers when a user program traps into the kernel.
struct trapframe {
  uint64 kernel_satp; // kernel page table
  uint64 kernel_sp;   // top of this process's kernel stack
  uint64 kernel_trap; // kernel C function that handles the trap
  uint64 epc;         // saved user program counter

  uint64 ra;
  uint64 sp;
  uint64 gp;
  uint64 tp;
  uint64 t0;
  uint64 t1;
  uint64 t2;
  uint64 s0;
  uint64 s1;
  uint64 a0;
  uint64 a1;
  uint64 a2;
  uint64 a3;
  uint64 a4;
  uint64 a5;
  uint64 a6;
  uint64 a7;
  uint64 s2;
  uint64 s3;
  uint64 s4;
  uint64 s5;
  uint64 s6;
  uint64 s7;
  uint64 s8;
  uint64 s9;
  uint64 s10;
  uint64 s11;
  uint64 t3;
  uint64 t4;
  uint64 t5;
  uint64 t6;
};

struct proc {
  enum procstate state;
  int pid;

  uint64 sz;
  pagetable_t pagetable;
  uint64 kstack;
  struct trapframe *trapframe;
  struct context context;

  struct proc *parent;
  uint64 wait_addr;
  int killed;
  int xstate;
  char name[16];
};
