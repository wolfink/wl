#include "util.h"

#ifndef LEXER_INTERNAL_H

typedef void Lexer;

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

#endif

extern Lexer*    lexer_create(Arena*, string* in);
extern string*   lexer_to_string(Arena*, Lexer*);
extern string*   lexer_get_value_at_index(Arena*, Lexer*, size_t index);
extern TokenType lexer_get_token_type_at_index(Lexer*, size_t index);
extern size_t    lexer_get_len(Lexer*);
