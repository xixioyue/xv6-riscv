.section .data
message:
  .string "Hello World, ToyOS is loading.\n"

.section .text
.global _entry
_entry:
  la t0, message

print_loop:
  lbu a0, 0(t0)
  beqz a0, setup_stack
  li a7, 1
  ecall
  addi t0, t0, 1
  j print_loop

setup_stack:
  la sp, stack0
  li a0, 4096
  add sp, sp, a0
  call start

spin:
  j spin

.section .bss
.align 16
stack0:
  .space 4096