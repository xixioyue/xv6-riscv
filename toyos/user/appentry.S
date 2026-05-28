.text
.global _start
_start:
  call main
  li a0, 0
  call exit

loop:
  j loop
