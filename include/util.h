#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <ctype.h>

#ifndef UTIL_INTERNAL_H
typedef void Arena;
#endif

typedef char string;

#define ARENA_MAX_SIZE MB(1)

#define vector_template(type)\
  typedef struct {\
    Arena* mem;\
    size_t size;\
    size_t len;\
    type* values;\
  } vector_##type;\
  vector_##type* vector_##type##_create(Arena* arena);\
  void vector_##type##_resize(vector_##type* vector);\
  void vector_##type##_add(vector_##type* vector, type value);\

#define vector_impl(type, default_size)\
  vector_##type* vector_##type##_create(Arena* arena)\
  {\
    vector_##type* out = arena_alloc(arena, sizeof(vector_##type));\
    out->mem = arena;\
    out->size = default_size;\
    out->len = 0;\
    out->values = arena_alloc(arena, sizeof(type) * default_size);\
    return out;\
  }\
  void vector_##type##_resize(vector_##type* vector)\
  {\
    if (vector->len >= vector->size) {\
      vector->size *= 2;\
      type* values_new = arena_alloc(vector->mem, sizeof(vector_##type) * vector->size);\
      memcpy(values_new, vector->values, vector->len);\
      vector->values = values_new;\
    }\
  }\
  void vector_##type##_add(vector_##type* vector, type value)\
  {\
    vector_##type##_resize(vector);\
    vector->values[vector->len++] = value;\
  }\

#define vector_create(type) vector_##type##_create
#define vector_add(type) vector_##type##_add

typedef void* object;
typedef int (*CTypeFunction)(char);

static inline int is_alpha(char c)  { return isalpha(c); }
static inline int is_alnum(char c)  { return isalnum(c); }
static inline int is_idchar(char c) { return isalnum(c) || c == '_' || c == '?'; }
static inline int is_digit(char c)  { return isdigit(c); }
static inline int is_float(char c)  { return isdigit(c); }
static inline int is_binary(char c) { return (c == '0' || c == '1'); }
static inline int is_octal(char c)  { return (c >= '0' && c < '8'); }
#define is_char(name, in) static inline int is_##name(char c) { return c == in; }
#define isn_char(name, in) static inline int isn_##name(char c) { return c != in; }
isn_char(dquote, '\"')
isn_char(quote, '\'')
isn_char(rpar, '(')
isn_char(rbrace, '}')
isn_char(semi, ';')
static inline int is_xdigit(char c) 
{ 
  return isdigit(c) || (c >= 'A' && c < 'G') || (c >= 'a' && c < 'g'); 
}

extern void die(const char*, ...);

extern Arena* arena_create();
// === Arena methods ======================================

extern Arena* arena_create_init(size_t initial_size);
extern void   arena_set_static(Arena*, int);
extern size_t arena_get_size(Arena*);
extern void   arena_print(Arena*);
extern void*  arena_alloc(Arena*, size_t bytes);
extern void   arena_free(Arena*);

// === Unsafe methods (use only with string literals!) =====
extern string*  u_strnew(Arena*, const char*);
// === Safe methods ========================================
extern string*  u_strnnew(Arena*, const char*, size_t num_chars);
extern int      u_strcpy(Arena*, string** restrict dest, const string* restrict src);
extern string*  u_strcpyar(Arena* new_arena, const string* str);
extern char     u_getc(const string*, size_t index);
extern size_t   u_strlen(const string*);
extern string*  u_strcat(Arena*, const string* restrict dest, const string* restrict src);
extern string*  u_strcats(Arena*, const string* restrict a, const char* restrict b);
extern int      u_strcmp(const string* a, const string* b);
extern string*  u_strslice(Arena*, const string*, size_t start, size_t end);
extern void     u_prints(const string*);

#endif
