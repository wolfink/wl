#ifndef UTIL_H
#define UTIL_H

#include<stdlib.h>

#ifndef UTIL_INTERNAL_H
typedef void String;
#endif

extern String* string_create(const char*);
extern int     string_copy(String* dest, const String* src);
extern char    string_get(const String*, size_t);
extern size_t  string_len(const String*);
extern String* string_combine(const String*, const String*);
extern int     string_addc(String*, char);
extern void    string_cat(String* dest, const String* src);
extern void    string_substr(String*, size_t start, size_t len);
extern int     string_compare(const String* a, const String* b);
extern String* string_match(const String*, const String* matcher);

//extern TokenVector* token_vector_create();
//extern void         token_vector_destroy(TokenVector*);
//extern void         token_vector_push_back(TokenVector*, String*);
//extern String*      token_vector_pop(TokenVector*);



#endif
