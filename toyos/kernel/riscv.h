#include "kernel/types.h"
#include "kernel/memlayout.h"

#define SATP_SV39 (8L << 60)

#define MAKE_SATP(pagetable) (SATP_SV39 | (((uint64)(pagetable)) >> 12))

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
w_satp(uint64 x)
{
  asm volatile("csrw satp, %0" : : "r"(x));
}

static inline void
sfence_vma(void)
{
  asm volatile("sfence.vma zero, zero");
}

static inline uint64
r_tp(void)
{
  uint64 x;
  asm volatile("mv %0, tp" : "=r"(x));
  return x;
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
