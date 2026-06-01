#define KERNBASE 0x80200000L

#define PHYSTOP (0x80000000L + 128 * 1024 * 1024)

#define PGSIZE 4096
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE - 1))
#define PGROUNDUP(sz) (((sz) + PGSIZE - 1) & ~(PGSIZE - 1))

#define USER_ENTRY 0x1000L
#define USER_INIT_STACK (USER_ENTRY + 2 * PGSIZE)
#define USERSTACK 1

#define TRAPFRAME_BASE 0x3fffffe000L

#define SSTATUS_SPP (1L << 8)
#define SSTATUS_SPIE (1L << 5)
#define SSTATUS_SUM (1L << 18)
