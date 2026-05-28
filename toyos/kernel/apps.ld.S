.section .rodata
.align 4

.global _binary_user_app_start
_binary_user_app_start:
  .incbin "user/app"

.global _binary_user_app_end
_binary_user_app_end:
