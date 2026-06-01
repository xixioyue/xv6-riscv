.section .data
message:
  .string "Hello World, ToyOS is loading.\n"

.section .text
.global _entry
_entry:
  la t0, message

print_loop:
  # OpenSBI 的 legacy console_putchar 调用约定：
  lbu a0, 0(t0)
  beqz a0, setup_stack
  li a7, 1
  ecall
  addi t0, t0, 1
  j print_loop

setup_stack:
  # 为 C 代码建立初始内核栈。栈从高地址向低地址增长，
  la sp, stack0
  li a0, 4096
  add sp, sp, a0
  call start

spin:
  # 正常情况下 start() 不会返回。若意外返回，则停留在此处，
  j spin

.section .bss
.align 16
stack0:
  .space 4096
.global kernel_stack_top
kernel_stack_top:
