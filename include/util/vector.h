#ifndef UTIL_VECTOR_H
#define UTIL_VECTOR_H

#include <stdlib.h>
#include <memory.h>
#include "arena.h"
#include "core.h"

#define vector_template(type)\
  typedef struct {\
    Arena* mem;\
    size_t size;\
    size_t len;\
    type* values;\
  } vector_##type;\
  vector_##type* vector_##type##_create(Arena* arena);\
  void vector_##type##_init(Arena* arena, vector_##type* v);\
  void vector_##type##_resize(vector_##type* vector);\
  void vector_##type##_add(vector_##type* vector, type value);\

#define vector_impl(type, default_size)\
  vector_##type* vector_##type##_create(Arena* arena)\
  {\
    vector_##type* out = arena_alloc(arena, sizeof(vector_##type));\
    vector_##type##_init(arena, out);\
    return out;\
  }\
  void vector_##type##_init(Arena* arena, vector_##type* v)\
  {\
    v->mem = arena;\
    v->size = default_size;\
    v->len = 0;\
    v->values = arena_alloc(arena, sizeof(type) * default_size);\
  }\
  void vector_##type##_resize(vector_##type* vector)\
  {\
    if (vector->len >= vector->size) {\
      vector->size *= 2;\
      type* values_new = arena_alloc(vector->mem, sizeof(type) * vector->size);\
      memcpy(values_new, vector->values, sizeof(type) * vector->len);\
      vector->values = values_new;\
    }\
  }\
  void vector_##type##_add(vector_##type* vector, type value)\
  {\
    vector_##type##_resize(vector);\
    vector->values[vector->len++] = value;\
  }\

#define vector_init(type) vector_##type##_init
#define vector_create(type) vector_##type##_create
#define vector_add(type) vector_##type##_add

vector_template(int);
vector_template(size_t);
vector_template(float);
vector_template(double);
vector_template(object);

#endif
