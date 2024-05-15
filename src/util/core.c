#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <memory.h>
#include <util/core.h>
#include <util/vector.h>
#include <util/hash_map.h>

vector_impl(int, 100);
vector_impl(float, 100);
vector_impl(double, 100);
vector_impl(size_t, 100);
vector_impl(object, 100);
vector_impl(Entry, 100);

int string_hash_map_compare(string* a, string* b)
{
  return u_strcmp(a, b) == 0;
}

size_t string_hash(string* in)
{
  string* in_str = in;
  int* in_int = (int*) in;
  size_t in_len = u_strlen(in_str);
  size_t out = 0;
  size_t i = 0; while(i++ < in_len / 8) out ^= in_int[i];
  out ^= in_int[i] & (-1 << (in_len % 8));
  return out;
}

hash_set_impl(string_ptr, string_hash, string_hash_map_compare)

// From suckless projects
void die(const char* errstr, ...)
{
  va_list ap;

  va_start(ap, errstr);
  vfprintf(stderr, errstr, ap);
  va_end(ap);
  exit(1);
}
