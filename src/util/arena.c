#include "util_internal.h"
#include "../util.h"

Arena* arena_create()
{
  return arena_create_init(DEFAULT_ARENA_SIZE);
}

Arena* arena_create_init(size_t initial_size)
{
  Arena* ret = malloc(sizeof(Arena));
  ret->size = initial_size;
  ret->location = 0;
  ret->data = malloc(initial_size);
  return ret;
}

size_t arena_get_size(Arena* a)
{
  return a->size;
}

size_t arena_get_used(Arena* a)
{
  return a->location;
}

void* arena_get_location(Arena* a, size_t loc)
{
  return a->data + loc;
}

void* arena_alloc(Arena* a, size_t bytes)
{
  if (a->location + bytes >= a->size) {
    a->data = realloc(a->data, a->size <<= 1);
    if (a->data == NULL) return NULL;
  }
  void* ptr = a->data + a->location;
  a->location += bytes;
  return ptr;
}

void arena_free(Arena* a)
{
  free(a->data);
  free(a);
}
