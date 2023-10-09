#ifndef UTIL_INTERNAL_H
#define UTIL_INTERNAL_H

#include <stdlib.h>

#define DEFAULT_ARENA_SIZE 1024
#define STR_SIZE(str) *(size_t*) (str - sizeof(str))

typedef char string;

typedef struct {
  size_t location, size;
  char* data;
} Arena;

#endif
