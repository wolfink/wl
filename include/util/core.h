#ifndef UTIL_CORE_H
#define UTIL_CORE_H

#include <ctype.h>

#define CHAR_SUM_2(a, b)                   (T(b) << 8)  + T(a)
#define CHAR_SUM_3(a, b, c)                (T(c) << 16) + (T(b) << 8)  + T(a)
#define CHAR_SUM_4(a, b, c, d)             (T(d) << 24) + (T(c) << 16) + (T(b) << 8)  + T(a)
#define CHAR_SUM_5(a, b, c, d, e)          (T(e) << 32) + (T(d) << 24) + (T(c) << 16) + (T(b) << 8)  + T(a)
#define CHAR_SUM_6(a, b, c, d, e, f)       (T(f) << 40) + (T(e) << 32) + (T(d) << 24) + (T(c) << 16) + (T(b) << 8)  + T(a)
#define CHAR_SUM_7(a, b, c, d, e, f, g)    (T(g) << 48) + (T(f) << 40) + (T(e) << 32) + (T(d) << 24) + (T(c) << 16) + (T(b) << 8)  + T(a)
#define CHAR_SUM_8(a, b, c, d, e, f, g, h) (T(h) << 56) + (T(g) << 48) + (T(f) << 40) + (T(e) << 32) + (T(d) << 24) + (T(c) << 16) + (T(b) << 8) + a

typedef void* object;
typedef int (*CTypeFunction)(char);

typedef enum {
  NONE,
  SOME
} Result;
#define optional_template(type) struct { Result result; type value; }
#define optional(type) optional_##type

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

#endif
