#ifndef UTIL_INTERNAL_H
#define UTIL_INTERNAL_H

#include <stdlib.h>

#define DEFAULT_ARENA_SIZE 1024
#define STR_SIZE(str) *(size_t*) (str - sizeof(size_t))
#define STR_PTR(size) (string*)(size + 1)

typedef char string;

typedef struct {
  size_t location, size;
  char* data;
} Arena;

#endif
