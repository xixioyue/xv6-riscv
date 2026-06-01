OUTPUT_ARCH("riscv")
ENTRY(_start)

/* 用户程序链接脚本：所有用户 ELF 都约定从虚拟地址 0x1000 开始 */
SECTIONS
{
  . = 0x1000;

  .text : {
    *(.text .text.*)
  }

  .rodata : {
    . = ALIGN(16);
    *(.rodata .rodata.*)
  }

  .data : {
    . = ALIGN(16);
    *(.data .data.*)
  }

  .bss : {
    . = ALIGN(16);
    *(.bss .bss.*)
  }
}
