#include "util.h"
#include<stdio.h>
#include<stdlib.h>
#include<err.h>

int main(int argc, char** argv)
{
  Arena* a = arena_create();

  if (argc < 2) {
    printf("Usage: wlc <filename>\n");
    return -1;
  }

  string* str = u_strnew(a, "Hello,");
  string* les = u_strnew(a, " world!\n");
  str = u_strcat(a, str, les);
  str = u_strcat(a, str, str);
  str = u_strcat(a, str, str);
  str = u_strcat(a, str, str);
  str = u_strcat(a, str, str);
  str = u_strcat(a, str, str);
  str = u_strcat(a, str, str);
  str = u_strcat(a, str, str);
  str = u_strcat(a, str, str);

  if (str == NULL) return 1;
  u_prints(str);
  printf("Arena size: %lu\n", arena_get_size(a));
  printf("Arena location: %lu\n", arena_get_loc(a));

  arena_free(a);
  return 0;
}
