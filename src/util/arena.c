#include "util_internal.h"
#include "../util.h"
#include <stdio.h>
#include <stdlib.h>

#define STATIC 1

int arena_node_init (ArenaNode* node, size_t size)
{
  node->location = 0;
  node->next = NULL;
  return posix_memalign((void**)&node->data, 64, size);
}

ArenaNode* arena_node_create(size_t size)
{
  ArenaNode* ret = malloc(sizeof(ArenaNode));
  if (arena_node_init(ret, size)) die("arena_node_create: failed to allocate ArenaNode memory\n");
  return ret;
}

Arena* arena_create()
{
  return arena_create_init(DEFAULT_ARENA_SIZE);
}

Arena* arena_create_init(size_t initial_size)
{
  Arena* ret = malloc(sizeof(Arena));
  if (!ret) die("arena_create_init: failed to allocate Arena memory\n");
  ret->size = initial_size;
  ret->first_size = initial_size;
  ret->flags = 0;
  if (arena_node_init(&ret->first, initial_size)) {
    die("arena_create_init: failed to allocated ArenaNode memory\n");
  }
  return ret;
}

void arena_set_static(Arena* a, int is_static)
{
  if (is_static) a->flags |= STATIC;
  else           a->flags = (a->flags | STATIC) - STATIC;
}

size_t arena_get_size(Arena* a)
{
  return a->size;
}

//size_t arena_get_used(Arena* a)
//{
//  return a->location;
//}

//void* arena_get_location(Arena* a, size_t loc)
//{
//  return a->data + loc;
//}

// For debugging
void arena_print(Arena* a)
{
  printf("first_size=%ld, size=%ld, flags=%ld\n", a->first_size, a->size, a->flags);
  ArenaNode* curr = &(a->first);
  while(curr) {
    printf("  data address=%p, location=%ld\n", curr->data, curr->location);
    curr = curr->next;
  }
}

void* arena_alloc(Arena* a, size_t bytes)
{
  ArenaNode* curr = &(a->first);
  int node_size = a->first_size;
  while (1)
  {
    if (curr->location + bytes < node_size) {
      void* ptr = curr->data + curr->location;
      curr->location += bytes;
      return ptr;
    }
    if (curr->next == NULL) break;
    node_size <<= 1;
    curr = curr->next;
  }

  if (a->flags & STATIC) die("arena_alloc: arena at full capacity: size=%ld, location=%ld", a->size, a->first.location + bytes);

  while((node_size <<= 1) <= bytes) ;

  curr->next = arena_node_create(node_size);
  a->size += node_size << 1;
  curr = curr->next;

  void* ptr = curr->data;
  curr->location += bytes;
  return ptr;
}

void arena_node_free(ArenaNode* n)
{
  if (!n) return;
  arena_node_free(n->next);
  free(n->data);
  free(n);
}

void arena_free(Arena* a)
{
  arena_node_free(a->first.next);
  free(a);
}

