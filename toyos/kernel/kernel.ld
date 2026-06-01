OUTPUT_ARCH("riscv")
ENTRY(_entry)

/* 内核链接脚本：OpenSBI 会把 ToyOS 加载到 0x80200000 */
SECTIONS
{
  . = 0x80200000;

  .text : {
    *(.text .text.*)
  }

  .rodata : {
    . = ALIGN(16);
    *(.rodata .rodata.*)
  }

  /* initcode 是内核启动后进入的第一段用户态代码 */
  .initcode : {
    . = ALIGN(16);
    *(.initcode)
  }
  
  .data : {
    . = ALIGN(16);
    *(.data .data.*)
  }

  .bss : {
    . = ALIGN(16);
    *(.bss .bss.*)
  }

  /* kalloc.c 从 end 之后开始回收空闲物理页 */
  PROVIDE(end = .);
}
