#include "kernel/defs.h"
#include "kernel/syscall.h"
#include "kernel/types.h"
#include "kernel/proc.h"
#include "kernel/vm.h"

static uint64
argraw(int n)
{
  struct trapframe *tf = current_proc()->trapframe;

  switch(n) {
  case 0:
    return tf->a0;
  case 1:
    return tf->a1;
  case 2:
    return tf->a2;
  case 3:
    return tf->a3;
  case 4:
    return tf->a4;
  case 5:
    return tf->a5;
  }

  return 0;
}

static uint64
sys_print(void)
{
  struct proc *p = current_proc();
  uint64 uva = argraw(0);
  char ch;

  for(int i = 0; i < 512; i++) {
    if(copyin(p->pagetable, &ch, uva + i, 1) < 0)
      return -1;
    if(ch == 0)
      return 0;
    console_putc(ch);
  }

  return -1;
}

static uint64
sys_fork(void)
{
  return fork_proc();
}

static uint64
sys_exit(void)
{
  exit_proc((int)argraw(0));
  return 0;
}

static uint64
sys_wait(void)
{
  return wait_proc(argraw(0));
}

static uint64
sys_exec(void)
{
  return exec_proc((int)argraw(0));
}

static uint64
sys_getpid(void)
{
  return current_proc()->pid;
}

// a7 保存系统调用号，a0 保存返回值。若系统调用改变了当前进程，
void
syscall(void)
{
  struct proc *p = current_proc();
  int num = p->trapframe->a7;
  uint64 ret;

  switch(num) {
  case SYS_print:
    ret = sys_print();
    break;
  case SYS_fork:
    ret = sys_fork();
    break;
  case SYS_exit:
    ret = sys_exit();
    break;
  case SYS_wait:
    ret = sys_wait();
    break;
  case SYS_exec:
    ret = sys_exec();
    break;
  case SYS_getpid:
    ret = sys_getpid();
    break;
  default:
    printf("unknown syscall %d\n", num);
    ret = -1;
    break;
  }

  if(current_proc() == p)
    p->trapframe->a0 = ret;
}
