#ifndef UTIL_HASH_SET_H
#define UTIL_HASH_SET_H


#include "vector.h"
#include <util/string.h>
#include <defs.h>

// typedef int (*CompareFunction)(object, object);
// typedef size_t (*HashFunction)(void*);

typedef struct { size_t key, val; } Entry;

vector_template(Entry)

#define HashMap(A, B) HashMap_##A##_##B
#define HASH_MAP_DEFAULT_SIZE 1000

#define hash_map_template(A, B)\
typedef optional_template(B) optional_##B;\
typedef struct {\
  vector_object keys;\
  size_t(*hash)(A);\
  int(*compare)(A, A);\
  Arena* mem;\
} HashMap(A, B);\
HashMap(A, B)* hash_map_##A##_##B##_new(Arena* arena);\
HashMap(A, B)* hash_map_##A##_##B##_create(Arena* arena, size_t size);\
HashMap(A, B)* hash_map_##A##_##B##_init(HashMap(A, B)* h, Arena* arena, size_t size);\
void hash_map_##A##_##B##_add(HashMap(A, B)*, A, B);\
optional(B) hash_map_##A##_##B##_get(HashMap(A, B)*, A);\
int hash_map_##A##_##B##_contains(HashMap(A, B)*, A);\

#define hash_map_new(A, B) hash_map_##A##_##B##_new
#define hash_map_create(A, B) hash_map_##A##_##B##_create
#define hash_map_init(A, B) hash_map_##A##_##B##_init
#define hash_map_add(A, B) hash_map_##A##_##B##_add
#define hash_map_get(A, B) hash_map_##A##_##B##_get
#define hash_map_contains(A, B) hash_map_##A##_##B##_contains

#define hash_map_impl(A, B, hash_fn, cmp_fn)\
HashMap(A, B)* hash_map_##A##_##B##_new(Arena* arena)\
{\
 return hash_map_##A##_##B##_create(arena, HASH_MAP_DEFAULT_SIZE);\
}\
HashMap(A, B)* hash_map_##A##_##B##_create(Arena* arena, size_t size)\
{\
 HashMap(A, B)* out = arena_alloc(arena, sizeof(HashMap(A, B)));\
 hash_map_##A##_##B##_init(out, arena, size);\
 return out;\
}\
HashMap(A, B)* hash_map_##A##_##B##_init(HashMap(A, B)* h, Arena* arena, size_t size)\
{\
  vector_object_init(arena, (vector_object*) h);\
  for (int i = 0; i < size; i++) {\
    vector_Entry* e = vector_Entry_create(arena);\
    NULL_CHECK(e, "hash_map_init(" #A ", " #B ")")\
    vector_object_add(&h->keys, e);\
  }\
  h->mem = arena;\
  h->hash = hash_fn;\
  h->compare = cmp_fn;\
  return h;\
}\
void hash_map_##A##_##B##_add(HashMap(A, B)* h, A a, B b)\
{\
  Entry e = {(size_t) a, (size_t) b};\
  vector_Entry_add((vector_Entry*) h->keys.values[h->hash(a) % h->keys.len], e);\
}\
optional(B) hash_map_##A##_##B##_get(HashMap(A, B)* h, A a)\
{\
  vector_Entry* bucket = h->keys.values[h->hash(a) % h->keys.len];\
  optional(B) out = { NONE, 0 };\
  for (int i = 0; i < bucket->len; i++) {\
    if (h->compare((A) bucket->values[i].key, a)) {\
      out.result = SOME;\
      out.value = (B) bucket->values[i].val;\
      return out;\
    }\
  }\
  return out;\
}\
int hash_map_##A##_##B##_contains(HashMap(A, B)* h, A a)\
{\
  vector_Entry* bucket = h->keys.values[h->hash(a) % h->keys.len];\
  for (int i = 0; i < bucket->len; i++)\
  {\
    if (h->compare((A) bucket->values[i].key, a)) return 1;\
  }\
  return 0;\
}\

#define HashSet(T) HashMap(T, int)
#define hash_set_template(A) hash_map_template(A, int)
#define hash_set_impl(A, hash_fn, cmp_fn) hash_map_impl(A, int, hash_fn, cmp_fn)

#define hash_set_new(T) hash_map_##T##_int_new
#define hash_set_create(T) hash_map_##T##_int_create
#define hash_set_init(T) hash_map_##T##_int_init
#define hash_set_add(T, hs, key) hash_map_##T##_int_add(hs, key, 1)
#define hash_set_get(T) hash_map_##T##_int_get
#define hash_set_contains(T) hash_map_##T##_int_contains

typedef string* string_ptr;
hash_set_template(string_ptr)

#endif
