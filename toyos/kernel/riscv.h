#include "kernel/types.h"
#include "kernel/memlayout.h"

static inline uint64
r_sstatus(void)
{
  uint64 x;
  asm volatile("csrr %0, sstatus" : "=r"(x));
  return x;
}

static inline void
w_sstatus(uint64 x)
{
  asm volatile("csrw sstatus, %0" : : "r"(x));
}

static inline uint64
r_scause(void)
{
  uint64 x;
  asm volatile("csrr %0, scause" : "=r"(x));
  return x;
}

static inline uint64
r_sepc(void)
{
  uint64 x;
  asm volatile("csrr %0, sepc" : "=r"(x));
  return x;
}

static inline void
w_sepc(uint64 x)
{
  asm volatile("csrw sepc, %0" : : "r"(x));
}

static inline void
w_stvec(uint64 x)
{
  asm volatile("csrw stvec, %0" : : "r"(x));
}

static inline void
intr_on(void)
{
  asm volatile("csrs sstatus, %0" : : "r"(2));
}

static inline void
intr_off(void)
{
  asm volatile("csrc sstatus, %0" : : "r"(2));
}