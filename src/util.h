#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <ctype.h>

#ifndef UTIL_INTERNAL_H
typedef void Arena;
typedef void string;
#endif

#define ARENA_MAX_SIZE MB(1)

typedef int (*CTypeFunction)(char);

static inline int is_alpha(char c)  { return isalpha(c); }
static inline int is_alnum(char c)  { return isalnum(c); }
static inline int is_digit(char c)  { return isdigit(c); }
static inline int is_float(char c)  { return isdigit(c); }
static inline int is_binary(char c) { return (c == '0' || c == '1'); }
static inline int is_octal(char c)  { return (c >= '0' && c < '8'); }
static inline int is_xdigit(char c) 
{ 
  return isdigit(c) || (c >= 'A' && c < 'G') || (c >= 'a' && c < 'g'); 
}

extern void die(const char*, ...);

extern Arena* arena_create();
extern Arena* arena_create_init(size_t initial_size);
extern void   arena_set_static(Arena*, int);
extern size_t arena_get_size(Arena*);
//extern size_t arena_get_used(Arena*);
//extern void*  arena_get_location(Arena*, size_t loc);
extern void   arena_print(Arena*);
//extern size_t arena_count_nodes(Arena*);
extern void*  arena_alloc(Arena*, size_t bytes);
extern void   arena_free(Arena*);

extern string*  u_strnew(Arena*, const string*);
extern int      u_strcpy(Arena*, string** restrict dest, const string* restrict src);
extern string*  u_strcpyar(Arena* new_arena, const string* str);
extern char     u_getc(const string*, size_t index);
extern size_t   u_strlen(const string*);
extern string*  u_strcat(Arena*, const string* restrict dest, const string* restrict src);
extern int      u_strcmp(const string* a, const string* b);
extern string*  u_strslice(Arena*, const string*, size_t start, size_t end);
extern void     u_prints(const string*);

#endif
