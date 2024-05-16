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

// From suckless projects
void die(const char* errstr, ...)
{
  va_list ap;

  va_start(ap, errstr);
  vfprintf(stderr, errstr, ap);
  va_end(ap);
  exit(1);
}
