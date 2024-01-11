#include "util_internal.h"
#include "../util.h"
#include <memory.h>
#include <stdio.h>
#include <string.h>

string* u_strnew(Arena* a, const string* in)
{
  if (!a) return NULL;
  size_t len = strlen(in);
  size_t* size = arena_alloc(a, sizeof(size_t) + len);
  string* ret = STR_PTR(size);
  memcpy(ret, in, len);
  *size = len;
  return ret;
}

int u_strcpy(Arena* a, string** dest, const string* src)
{
  if (!a) return 1;
  if (!dest) return 2;
  if (!src) return 3;
  if (!STR_SIZE(*dest)) {
    size_t* size = arena_alloc(a, sizeof(size_t) + STR_SIZE(src));
    if(!size) return 4;
    *dest = STR_PTR(size);
  }

  memcpy(*dest, src, STR_SIZE(src));
  STR_SIZE(*dest) = STR_SIZE(src);

  return 0;
}

string* u_strcpyar(Arena* a, const string* src)
{
  if (!src) return NULL;
  size_t* size = arena_alloc(a, sizeof(size_t) + STR_SIZE(src));
  if(!size) return NULL;
  string* dest = STR_PTR(size);

  memcpy(dest, src, STR_SIZE(src));
  *size = STR_SIZE(src);

  return dest;
}

char u_getc(const string* str, size_t index)
{
  if (index >= STR_SIZE(str)) return '\0';
  return str[index];
}

size_t u_strlen(const string* str)
{
  return STR_SIZE(str);
}

string* u_strcat(Arena* arena, const string* restrict a, const string* restrict b)
{
  if (!arena) return NULL;
  if (!a) return NULL;
  if (!b) return NULL;
  const size_t new_len = STR_SIZE(a) + STR_SIZE(b);
  size_t* size = arena_alloc(arena, sizeof(size_t) + new_len);
  string* ret = (string*)(size + 1);

  if (!ret) return NULL;

  memcpy(ret, a, STR_SIZE(a));
  memcpy(ret + STR_SIZE(a), b, STR_SIZE(b));
  *size = new_len;

  return ret;
}

int u_strcmp(const string* a, const string* b)
{
  if (!a) return NULL;
  if (!b) return NULL;
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

string* u_strslice(Arena* arena, const string* in, size_t start, size_t end)
{
  if (!arena || !in) return NULL;
  size_t len = end - start - 1;
  len = (len > u_strlen(in) - start - 1) ? u_strlen(in) - start: len;
  size_t* size = arena_alloc(arena, sizeof(size_t) + len);
  string* ret = STR_PTR(size);
  memcpy(ret, in + start, len);
  *size = len;
  return ret;
}

void u_prints(const string* str)
{
  if (!str) return;
  size_t len = STR_SIZE(str);
  for(int i = 0; i < len; i++)
    putchar(str[i]);
}
