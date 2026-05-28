#include "kernel/types.h"

typedef uint64 pte_t;
typedef uint64 *pagetable_t;

#define PTE_V (1L << 0)
#define PTE_R (1L << 1)
#define PTE_W (1L << 2)
#define PTE_X (1L << 3)
#define PTE_U (1L << 4)

#define PA2PTE(pa) ((((uint64)(pa)) >> 12) << 10)
#define PTE2PA(pte) (((pte) >> 10) << 12)
#define PTE_FLAGS(pte) ((pte) & 0x3ff)

#define PXSHIFT(level) (12 + (9 * (level)))
#define PX(level, va) ((((uint64)(va)) >> PXSHIFT(level)) & 0x1ff)
#define MAXVA (1L << (9 + 9 + 9 + 12 - 1))
