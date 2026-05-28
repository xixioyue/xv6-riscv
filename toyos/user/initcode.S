.text
.global _start
_start:
  call printf_userspace

loop:
  j loop