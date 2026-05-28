#include "kernel/memlayout.h"

.section .text

.global uservec
uservec:
  csrw sscratch, a0
  li a0, TRAPFRAME_BASE

  sd ra, 32(a0)
  sd sp, 40(a0)
  sd gp, 48(a0)
  sd tp, 56(a0)
  sd t0, 64(a0)
  sd t1, 72(a0)
  sd t2, 80(a0)
  sd s0, 88(a0)
  sd s1, 96(a0)
  sd a1, 112(a0)
  sd a2, 120(a0)
  sd a3, 128(a0)
  sd a4, 136(a0)
  sd a5, 144(a0)
  sd a6, 152(a0)
  sd a7, 160(a0)
  sd s2, 168(a0)
  sd s3, 176(a0)
  sd s4, 184(a0)
  sd s5, 192(a0)
  sd s6, 200(a0)
  sd s7, 208(a0)
  sd s8, 216(a0)
  sd s9, 224(a0)
  sd s10, 232(a0)
  sd s11, 240(a0)
  sd t3, 248(a0)
  sd t4, 256(a0)
  sd t5, 264(a0)
  sd t6, 272(a0)

  csrr t0, sscratch
  sd t0, 104(a0)

  ld sp, 8(a0)
  ld t0, 16(a0)
  ld t1, 0(a0)
  csrw satp, t1
  sfence.vma zero, zero
  jr t0

.global userret
userret:
  call user_satp
  csrw satp, a0
  sfence.vma zero, zero

  li a0, TRAPFRAME_BASE

  ld ra, 32(a0)
  ld sp, 40(a0)
  ld gp, 48(a0)
  ld tp, 56(a0)
  ld t0, 64(a0)
  ld t1, 72(a0)
  ld t2, 80(a0)
  ld s0, 88(a0)
  ld s1, 96(a0)
  ld a1, 112(a0)
  ld a2, 120(a0)
  ld a3, 128(a0)
  ld a4, 136(a0)
  ld a5, 144(a0)
  ld a6, 152(a0)
  ld a7, 160(a0)
  ld s2, 168(a0)
  ld s3, 176(a0)
  ld s4, 184(a0)
  ld s5, 192(a0)
  ld s6, 200(a0)
  ld s7, 208(a0)
  ld s8, 216(a0)
  ld s9, 224(a0)
  ld s10, 232(a0)
  ld s11, 240(a0)
  ld t3, 248(a0)
  ld t4, 256(a0)
  ld t5, 264(a0)
  ld t6, 272(a0)
  ld a0, 104(a0)

  sret
