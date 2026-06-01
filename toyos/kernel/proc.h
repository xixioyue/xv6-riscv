#ifndef TOYOS_KERNEL_PROC_H
#define TOYOS_KERNEL_PROC_H

#include "kernel/types.h"
#include "kernel/vm.h"

#define NPROC 8

enum procstate {
  UNUSED,
  USED,
  SLEEPING,
  RUNNABLE,
  RUNNING,
  ZOMBIE,
};

struct context {
  /*  0 */ uint64 ra;
  /*  8 */ uint64 sp;
  /* 16 */ uint64 s0;
  /* 24 */ uint64 s1;
  /* 32 */ uint64 s2;
  /* 40 */ uint64 s3;
  /* 48 */ uint64 s4;
  /* 56 */ uint64 s5;
  /* 64 */ uint64 s6;
  /* 72 */ uint64 s7;
  /* 80 */ uint64 s8;
  /* 88 */ uint64 s9;
  /* 96 */ uint64 s10;
  /* 104 */ uint64 s11;
};

// 用户态发生 trap 时保存的寄存器现场
// trampoline.S 使用固定偏移访问这些字段
struct trapframe {
  /*   0 */ uint64 kernel_satp; // 内核页表对应的 satp 值
  /*   8 */ uint64 kernel_sp;   // 进入内核态后使用的栈顶
  /*  16 */ uint64 kernel_trap; // 内核 C 入口 usertrap()
  /*  24 */ uint64 epc;         // 保存的用户态程序计数器
  /*  32 */ uint64 kernel_hartid;
  /*  40 */ uint64 ra;
  /*  48 */ uint64 sp;
  /*  56 */ uint64 gp;
  /*  64 */ uint64 tp;
  /*  72 */ uint64 t0;
  /*  80 */ uint64 t1;
  /*  88 */ uint64 t2;
  /*  96 */ uint64 s0;
  /* 104 */ uint64 s1;
  /* 112 */ uint64 a0;
  /* 120 */ uint64 a1;
  /* 128 */ uint64 a2;
  /* 136 */ uint64 a3;
  /* 144 */ uint64 a4;
  /* 152 */ uint64 a5;
  /* 160 */ uint64 a6;
  /* 168 */ uint64 a7;
  /* 176 */ uint64 s2;
  /* 184 */ uint64 s3;
  /* 192 */ uint64 s4;
  /* 200 */ uint64 s5;
  /* 208 */ uint64 s6;
  /* 216 */ uint64 s7;
  /* 224 */ uint64 s8;
  /* 232 */ uint64 s9;
  /* 240 */ uint64 s10;
  /* 248 */ uint64 s11;
  /* 256 */ uint64 t3;
  /* 264 */ uint64 t4;
  /* 272 */ uint64 t5;
  /* 280 */ uint64 t6;
};

struct proc {
  // 调度
  enum procstate state;   // 进程状态
  int pid;                // 进程 ID
  struct context context; // 进程上下文寄存器
  
  // 内存管理
  uint64 sz;              // 进程地址空间大小
  pagetable_t pagetable;  // 用户页表
  uint64 kstack;          // 内核栈虚拟地址
  struct trapframe *trapframe; //陷阱帧

  // 父子关系
  struct proc *parent;    // 父进程指针
  void *chan;             // 若非零，则进程正在该通道上睡眠

  // 其他
  int killed;             // 进程是否被杀死，非0则被杀死
  int xstate;             // 进程退出状态
  char name[16];          // 进程名称
};

#endif
