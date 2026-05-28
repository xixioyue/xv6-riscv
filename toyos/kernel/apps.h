#include "kernel/types.h"

struct app_entry {
  const char *name;
  const uchar *start;
  uint64 size;
};

extern struct app_entry apps_table[];
extern uint64 apps_count;
