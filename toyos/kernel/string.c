#include "kernel/defs.h"

void *
memmove(void *dst, const void *src, uint n)
{
  const char *s;
  char *d;

  if(n == 0)
    return dst;

  s = src;
  d = dst;
  if(s < d && s + n > d) {
    s += n;
    d += n;
    while(n-- > 0)
      *--d = *--s;
  } else {
    while(n-- > 0)
      *d++ = *s++;
  }

  return dst;
}