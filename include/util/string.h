#ifndef UTIL_STRING_H
#define UTIL_STRING_H

#include "arena.h"
#include <stdio.h>
typedef char string;

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
extern string*  u_read_file(Arena* context, FILE* fp);

#endif
