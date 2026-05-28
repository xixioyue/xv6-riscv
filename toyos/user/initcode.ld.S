.section .initcode, "a"
.global _binary_user_initcode_bin_start
.global _binary_user_initcode_bin_end
.global _binary_user_initcode_bin_size

_binary_user_initcode_bin_start:
  .incbin "user/initcode.bin"
_binary_user_initcode_bin_end:

_binary_user_initcode_bin_size = _binary_user_initcode_bin_end - _binary_user_initcode_bin_start