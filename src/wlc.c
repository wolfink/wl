#include "util.h"
#include<stdio.h>
#include<stdlib.h>
#include<err.h>

int main(int argc, char** argv)
{
  if (argc < 2) {
    printf("Usage: wlc <filename>\n");
    return -1;
  }

  String* str = string_create("Hello,");
  String* les = string_create(" world!\n");
  string_cat(str, les);
  if (str == NULL) return 1;
  for(int i = 0; i < string_len(str); i++)
  {
    putchar(string_get(str, i));
  }
  return 0;
}
