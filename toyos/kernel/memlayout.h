// QEMU virt + OpenSBI loads our kernel at 0x80200000.
#define KERNBASE 0x80200000L

// Keep the first user program away from the kernel image.
#define USER_BASE 0x80400000L
#define USER_STACK (USER_BASE + 2 * PGSIZE)

// ToyOS currently runs with 128 MiB of RAM.
#define PHYSTOP (0x80000000L + 128 * 1024 * 1024)

#define PGSIZE 4096
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE - 1))
#define PGROUNDUP(sz) (((sz) + PGSIZE - 1) & ~(PGSIZE - 1))

// Chapter 6 moves user programs into per-process virtual address spaces.
#define USER_ENTRY 0x1000L
#define USER_INIT_STACK (USER_ENTRY + 2 * PGSIZE)
#define USERSTACK 1

// Sv39's usable positive virtual range ends at 0x4000000000.
// Map each process's trapframe here, outside the kernel direct map.
#define TRAPFRAME_BASE 0x3fffffe000L

#define SSTATUS_SPP (1L << 8)
#define SSTATUS_SPIE (1L << 5)
#define SSTATUS_SUM (1L << 18)
