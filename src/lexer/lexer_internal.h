#ifndef LEXER_INTERNAL_H
#define LEXER_INTERNAL_H

#include "lexer_shared.h"
#include <util.h>
#include <sys/types.h>

#define MAX_TOKEN_LEN KB(1) - 1
#define LEXER_DEFAULT_SIZE KB(80)


u_int64_t token_type_number[TokenType_COUNT] = {
#define T(a) ((u_int64_t)a)
#define X(name, number, str) number,
  TokenTypeTable
#undef X
#undef T
};

char *token_type_str[TokenType_COUNT] = {
#define X(name, first, str) str,
  TokenTypeTable
#undef X
};

// typedef struct
// {
//   size_t size;
//   size_t len;
//   size_t strlen;
//   TokenType* tokens;
//   string** token_strings; // TODO: instead of direct pointers, store offsets from base of Arena
// } Lexer;

vector_template(Token);
typedef vector_Token Line;

typedef vector_object vector_Line;

string* line_to_string(Arena*, const Line* line);

typedef struct {
  Arena* mem;
  size_t curr_line;
  const char* filename;
  vector_Line* lines;
} Lexer;

#endif
