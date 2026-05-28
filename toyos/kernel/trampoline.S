#include "kernel/memlayout.h"

.section .text

.global switch_to_user
switch_to_user:
  li t0, USER_BASE
  csrw sepc, t0
  li sp, USER_STACK

  csrr t0, sstatus
  li t1, SSTATUS_SPP
  not t1, t1
  and t0, t0, t1
  li t1, SSTATUS_SPIE
  or t0, t0, t1
  csrw sstatus, t0

  sret

.global uservec
uservec:
  csrw sscratch, a0
  li a0, TRAPFRAME_BASE

  sd ra, 16(a0)
  sd sp, 24(a0)
  sd gp, 32(a0)
  sd tp, 40(a0)
  sd t0, 48(a0)
  sd t1, 56(a0)
  sd t2, 64(a0)
  sd s0, 72(a0)
  sd s1, 80(a0)
  sd a1, 96(a0)
  sd a2, 104(a0)
  sd a3, 112(a0)
  sd a4, 120(a0)
  sd a5, 128(a0)
  sd a6, 136(a0)
  sd a7, 144(a0)
  sd s2, 152(a0)
  sd s3, 160(a0)
  sd s4, 168(a0)
  sd s5, 176(a0)
  sd s6, 184(a0)
  sd s7, 192(a0)
  sd s8, 200(a0)
  sd s9, 208(a0)
  sd s10, 216(a0)
  sd s11, 224(a0)
  sd t3, 232(a0)
  sd t4, 240(a0)
  sd t5, 248(a0)
  sd t6, 256(a0)

  csrr t0, sscratch
  sd t0, 88(a0)

  la sp, kernel_stack_top

  ld t0, 0(a0)
  jr t0

.global userret
userret:
  li a0, TRAPFRAME_BASE

  ld ra, 16(a0)
  ld sp, 24(a0)
  ld gp, 32(a0)
  ld tp, 40(a0)
  ld t0, 48(a0)
  ld t1, 56(a0)
  ld t2, 64(a0)
  ld s0, 72(a0)
  ld s1, 80(a0)
  ld a1, 96(a0)
  ld a2, 104(a0)
  ld a3, 112(a0)
  ld a4, 120(a0)
  ld a5, 128(a0)
  ld a6, 136(a0)
  ld a7, 144(a0)
  ld s2, 152(a0)
  ld s3, 160(a0)
  ld s4, 168(a0)
  ld s5, 176(a0)
  ld s6, 184(a0)
  ld s7, 192(a0)
  ld s8, 200(a0)
  ld s9, 208(a0)
  ld s10, 216(a0)
  ld s11, 224(a0)
  ld t3, 232(a0)
  ld t4, 240(a0)
  ld t5, 248(a0)
  ld t6, 256(a0)
  ld a0, 88(a0)

  sret
