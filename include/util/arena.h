#ifndef UTIL_ARENA_H
#define UTIL_ARENA_H

#include <stdlib.h>

#ifndef ARENA_INTERNAL

typedef struct {} Arena;

#else

typedef struct ArenaNode {
  char* data;
  size_t location;
  struct ArenaNode* next;
} ArenaNode;

typedef struct {
  size_t first_size, size, flags;
  ArenaNode first;
} Arena;

#endif

#define ARENA_MAX_SIZE MB(1)

extern Arena* arena_create();
extern Arena* arena_create_init(size_t initial_size);
extern void   arena_set_static(Arena*, int);
extern size_t arena_get_size(Arena*);
extern void   arena_print(Arena*);
extern void*  arena_alloc(Arena*, size_t bytes);
extern void   arena_free(Arena*);

#endif
