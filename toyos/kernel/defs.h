#include "kernel/types.h"
#include "kernel/proc.h"

int sbi_call(uint64 which, uint64 arg0, uint64 arg1, uint64 arg2);
void console_putc(int c);
void printf(const char *fmt, ...);

void start(void);
void main(void);
