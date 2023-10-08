#include "util_internal.h"
#include "../util.h"
#include <memory.h>
#include <stdio.h>
#include <string.h>

String* string_create(const char* in)
{
  size_t len = strlen(in);
  String* ret = (String*) malloc(sizeof(String));
  if (ret == NULL) return ret;
  ret->val = (char*) malloc(sizeof(char) * len);
  memcpy(ret->val, in, len);
  ret->len = len;
  return ret;
}

int string_copy(String* dest, const String* src)
{
  if (dest != NULL) free(dest);

  dest = string_create("");
  if (dest == NULL) return 1;

  for (int i = 0; i < src->len; i++) string_addc(dest, src->val[i]);
  return 0;
}

int string_addc(String* str, char c)
{
  char* new_char_arr = (char*) realloc(str->val, sizeof(char) * ++str->len);
  if (new_char_arr == NULL) return 1;
  new_char_arr[str->len - 1] = c;
  return 0;
}

char string_get(const String* str, size_t index)
{
  return str->val[index];
}

size_t string_len(const String* str)
{
  return str->len;
}

String* string_combine(const String* a, const String* b)
{
  String *new_string;
  string_copy(new_string, a);
  string_cat(new_string, b);
  return new_string;
}

void string_cat(String* dest, const String* src)
{
  size_t new_len = src->len + dest->len;
  dest->val = realloc(dest->val, sizeof(char) * new_len);
  if (dest->val == NULL) return;
  memcpy(dest->val + dest->len, src->val, src->len);
  dest->len = new_len;
}

void string_substr(String* str, size_t start, size_t len)
{
  str->val += start;
  char* new_val = (char*) malloc(sizeof(char) * len);
  memcpy(new_val, str->val, len);
  free(str->val);
  str->val = new_val;
}

int string_compare(const String* a, const String* b)
{
  if (a->len < b->len) return -b->val[a->len];
  if (a->len > b->len) return a->val[b->len];

  for(int i = 0; i < a->len && i < b->len; i++)
  {
    int str_diff = a->val[i] - b->val[i];
    if (str_diff != 0) return str_diff;
  }

  return 0;
}
