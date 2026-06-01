#include "kernel/defs.h"
#include "kernel/elf.h"
#include "kernel/memlayout.h"
#include "kernel/vm.h"
#include "kernel/types.h"
#include "kernel/riscv.h"

static int
flags2perm(int flags)
{
  int perm = PTE_R | PTE_U;

  if(flags & 0x1)
    perm |= PTE_X;
  if(flags & 0x2)
    perm |= PTE_W;
    
  return perm;
}

static int
loadseg_mem(pagetable_t pagetable, uint64 va, const char *buf, uint off, uint sz)
{
  uint i;

  for(i = 0; i < sz; i += PGSIZE) {
    uint64 pa = walkaddr(pagetable, va + i);
    if(pa == 0) {
      printf("elf: loadseg_mem: missing page");
      return -1;
    }

    uint n = (sz - i < PGSIZE) ? sz - i : PGSIZE;
    memmove((void *)pa, buf + off + i, n);
    if(n < PGSIZE)
      memset((void *)(pa + n), 0, PGSIZE - n);
  }

  return 0;
}

int
elf_load_from_mem(pagetable_t pagetable, const void *buf, uint64 bufsz,
                  uint64 *entry_out, uint64 *out_sz)
{
  struct elfhdr elf;
  struct proghdr ph;
  uint64 off;
  int i;
  uint64 sz = 0;

  if(bufsz < sizeof(elf))
    return -1;

  memmove(&elf, buf, sizeof(elf));
  if(elf.magic != ELF_MAGIC)
    return -1;

  for(i = 0, off = elf.phoff; i < elf.phnum; i++, off += sizeof(ph)) {
    if(off + sizeof(ph) > bufsz)
      return -1;

    memmove(&ph, (char *)buf + off, sizeof(ph));
    if(ph.type != ELF_PROG_LOAD)
      continue;

    if(ph.memsz < ph.filesz)
      return -1;
    if(ph.vaddr + ph.memsz < ph.vaddr)
      return -1;
    if(ph.vaddr % PGSIZE != 0)
      return -1;

    uint64 newsz = uvmalloc(pagetable, sz, ph.vaddr + ph.memsz, flags2perm(ph.flags));
    if(newsz == 0)
      return -1;

    if(loadseg_mem(pagetable, ph.vaddr, (const char *)buf, ph.off, ph.filesz) < 0)
      return -1;

    sz = newsz;
  }

  if(entry_out)
    *entry_out = elf.entry;
  if(out_sz)
    *out_sz = sz;

  return 0;
}
