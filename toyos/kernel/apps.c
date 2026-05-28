#include "kernel/apps.h"

extern uchar _binary_user_app_start[];
extern uchar _binary_user_app_end[];

struct app_entry apps_table[1];
uint64 apps_count;

void
appsinit(void)
{
  apps_table[0].name = "app";
  apps_table[0].start = _binary_user_app_start;
  apps_table[0].size = (uint64)(_binary_user_app_end - _binary_user_app_start);
  apps_count = 1;
}
