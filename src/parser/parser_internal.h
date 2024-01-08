#ifndef PARSER_INTERNAL_H
#define PARSER_INTERNAL_H

#include "../util.h"
#include "../lexer.h"
#include "parser_shared.h"
#include "ast.h"

typedef struct
{
  size_t token_idx;
  Lexer* lexer;
  AST* tree;
} Parser;

AST* scan(Arena*, Parser*, TokenType);
TokenType next_token(Parser* p) { return lexer_get_token_type_at_index(p->lexer, p->token_idx); }

#endif
