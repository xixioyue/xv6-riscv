#include "kernel/defs.h"

int
sbi_call(uint64 which, uint64 arg0, uint64 arg1, uint64 arg2)
{
  register uint64 a0 asm("a0") = arg0;
  register uint64 a1 asm("a1") = arg1;
  register uint64 a2 asm("a2") = arg2;
  register uint64 a7 asm("a7") = which;

  asm volatile("ecall"
    : "=r"(a0)
    : "r"(a0), "r"(a1), "r"(a2), "r"(a7)
    : "memory");

  return a0;
}