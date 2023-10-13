#ifndef LEXER_INTERNAL_H
#define LEXER_INTERNAL_H

#include "../util.h"
#include <string.h>
#include <ctype.h>

#define MAX_TOKEN_LEN KB(1) - 1
#define LEXER_DEFAULT_SIZE KB(80)

#define TokenTypeTableSimple \
  X(SEMI,         ';', "SEMI") \
  X(LBRACE,       '{', "LBRACE")\
  X(RBRACE,       '}', "RBRACE")\
  X(LPAR,         '(', "LPAR")\
  X(RPAR,         ')', "RPAR")\
  X(PERIOD,       '.', "PERIOD")\
  X(STAR,         '*', "STAR")\
  X(PLUS,         '+', "PLUS")\
  X(CARAT,        '^', "CARAT")\
  X(AND,          '&', "AND")\
  X(COMMA,        ',', "COMMA")\
  X(TYPE_ASSIGN,  ':', "COLON")\

#define TokenTypeTableOverload \
  X(MINUS,        '-', "MINUS")\
  X(SMALLARROW,   '-', "S-ARROW")\
  X(ASSIGN,       '=', "ASSIGN")\
  X(BIGARROW,     '=', "B-ARROW")\
  X(EQUALS,       '=', "EQUALS")\

#define TokenTypeTableStmts \
  X(NUMBER,      '\0', "NUM")\
  X(ID,          '\0', "ID")\

#define TokenTypeTable \
  TokenTypeTableSimple\
  TokenTypeTableOverload\
  TokenTypeTableStmts

typedef enum {
#define X(name, first, str) TokenType_##name,
  TokenTypeTable
#undef X
  TokenType_COUNT
} TokenType;

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
