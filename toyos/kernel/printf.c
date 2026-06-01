#include <stdarg.h>
#include "kernel/defs.h"

static void
printstr(const char *s)
{
  if(s == 0)
    s = "(null)";

  while(*s)
    console_putc(*s++);
}

static void
printint(long x, int base, int sign)
{
  static char digits[] = "0123456789abcdef";
  char buf[32];
  unsigned long y;
  int i = 0;

  if(sign && x < 0) {
    console_putc('-');
    y = -x;
  } else {
    y = x;
  }

  do {
    buf[i++] = digits[y % base];
    y /= base;
  } while(y != 0);

  while(--i >= 0)
    console_putc(buf[i]);
}

void
printf(const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  for(; *fmt; fmt++) {
    if(*fmt != '%') {
      console_putc(*fmt);
      continue;
    }

    fmt++;
    if(*fmt == 0)
      break;

    switch(*fmt) {
    case 'c':
      console_putc(va_arg(ap, int));
      break;
    case 'd':
      printint(va_arg(ap, int), 10, 1);
      break;
    case 'x':
      printint(va_arg(ap, int), 16, 0);
      break;
    case 'p':
      printstr("0x");
      printint(va_arg(ap, uint64), 16, 0);
      break;
    case 's':
      printstr(va_arg(ap, char *));
      break;
    case '%':
      console_putc('%');
      break;
    default:
      console_putc('%');
      console_putc(*fmt);
      break;
    }
  }
  va_end(ap);
}
