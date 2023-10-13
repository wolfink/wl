#ifndef UTIL_H
#define UTIL_H

#include<stdlib.h>

#ifndef UTIL_INTERNAL_H
typedef void Arena;
typedef void string;
#endif

#define ARENA_MAX_SIZE MB(1)

extern Arena* arena_create();
extern Arena* arena_create_init(size_t initial_size);
extern void   arena_set_static(Arena*, int);
extern size_t arena_get_size(Arena*);
extern size_t arena_get_used(Arena*);
extern void*  arena_get_location(Arena*, size_t loc);
extern void*  arena_alloc(Arena*, size_t bytes);
extern void   arena_free(Arena*);

extern string*  u_strnew(Arena*, const string*);
extern int      u_strcpy(Arena*, string* restrict dest, const string* restrict src);
extern string*  u_strcpyar(Arena* new_arena, const string* str);
extern char     u_getc(const string*, size_t index);
extern size_t   u_strlen(const string*);
extern string*  u_strcat(Arena*, const string* restrict dest, const string* restrict src);
extern int      u_strcmp(const string* a, const string* b);
extern string*  u_strslice(Arena*, const string*, size_t start, size_t end);
extern void     u_prints(const string*);

//extern TokenVector* token_vector_create();
//extern void         token_vector_destroy(TokenVector*);
//extern void         token_vector_push_back(TokenVector*, String*);
//extern String*      token_vector_pop(TokenVector*);



#endif
