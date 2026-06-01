.text
.global _start

_start:
  li a0, 0
  call exec

  li a0, 1
  call exit

loop:
  j loop
