#ifndef PARSER_INTERNAL_H
#define PARSER_INTERNAL_H

#include <util.h>
#include <lexer.h>
#include "ast.h"

typedef struct
{
  Arena* context;
  size_t token_idx;
  Lexer* lexer;
} Parser;

AST* scan(Arena*, Parser*, TokenType);
TokenType next_token(Parser* p) { return lexer_get_token_type_at_index(p->lexer, p->token_idx); }

#endif
