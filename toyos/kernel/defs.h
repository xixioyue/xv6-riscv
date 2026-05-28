#include "kernel/types.h"
#include "kernel/proc.h"

int sbi_call(uint64 which, uint64 arg0, uint64 arg1, uint64 arg2);
void console_putc(int c);
void printf(const char *fmt, ...);

void start(void);
void main(void);

void *memmove(void *dst, const void *src, uint n);
void trap_init(void);
void load_user_program(void);
void enter_user_space(void);
void usertrap(void);
void syscall(void);