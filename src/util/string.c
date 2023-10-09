#include "util_internal.h"
#include "../util.h"
#include <memory.h>
#include <stdio.h>
#include <string.h>
#define STR_SIZE(str) *(size_t*) (str - sizeof(str))

char* u_strnew(Arena* a, const char* in)
{
  size_t len = strlen(in);
  size_t* size = arena_alloc(a, sizeof(size_t) + len);
  char* ret = (char*) (size + 1);
  memcpy(ret, in, len);
  *size = len;
  return ret;
}

int u_strcpy(Arena* a, char* restrict dest, const char* restrict src)
{
  if (!dest) dest = arena_alloc(a, sizeof(size_t) + STR_SIZE(src));

  memcpy(dest, src, STR_SIZE(src));


  return 0;
}

char u_getc(const char* str, size_t index)
{
  if (index >= STR_SIZE(str)) return '\0';
  return str[index];
}

size_t u_strlen(const char* str)
{
  return STR_SIZE(str);
}

char* u_strcat(Arena* arena, const char* restrict a, const char* restrict b)
{
  const size_t new_len = STR_SIZE(a) + STR_SIZE(b);
  size_t* size = arena_alloc(arena, sizeof(size_t) + new_len);
  char* ret = (char*)(size + 1);

  if (!ret) return NULL;

  memcpy(ret, a, STR_SIZE(a));
  memcpy(ret + STR_SIZE(a), b, STR_SIZE(b));
  *size = new_len;

  return ret;
}

int u_strcmp(const char* a, const char* b)
{
  const size_t alen = STR_SIZE(a);
  const size_t blen = STR_SIZE(b);

  if (alen < blen) return -b[alen];
  if (alen > blen) return a[blen];

  for(int i = 0; i < alen; i++)
  {
    int str_diff = a[i] - b[i];
    if (str_diff != 0) return str_diff;
  }

  return 0;
}

void u_prints(const char* str)
{
  for(int i = 0; i < STR_SIZE(str); i++) putchar(str[i]);
}
