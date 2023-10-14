#ifndef LEXER_INTERNAL_H
#define LEXER_INTERNAL_H

#include "lexer_shared.h"
#include "../util.h"
#include <string.h>
#include <ctype.h>

#define MAX_TOKEN_LEN KB(1) - 1
#define LEXER_DEFAULT_SIZE KB(80)

char token_type_first[TokenType_COUNT] = {
#define X(name, first, str) first,
  TokenTypeTable
#undef X
};

char *token_type_str[TokenType_COUNT] = {
#define X(name, first, str) str,
  TokenTypeTable
#undef X
};

typedef struct
{
  size_t size;
  size_t len;
  size_t strlen;
  TokenType* tokens;
  string** token_strings; // TODO: instead of direct pointers, store offsets from base of Arena
} Lexer;

#endif
