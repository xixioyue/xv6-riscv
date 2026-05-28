// QEMU virt + OpenSBI loads our kernel at 0x80200000.
#define KERNBASE 0x80200000L

// Keep the first user program away from the kernel image.
#define USER_BASE 0x80400000L
#define USER_STACK (USER_BASE + 2 * PGSIZE)

// ToyOS currently runs with 128 MiB of RAM.
#define PHYSTOP (0x80000000L + 128 * 1024 * 1024)

#define PGSIZE 4096
#define TRAPFRAME_BASE (PHYSTOP - PGSIZE)

#define SSTATUS_SPP (1L << 8)
#define SSTATUS_SPIE (1L << 5)
