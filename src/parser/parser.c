#include "ast.h"
#include "parser_internal.h"
#include "../parser.h"
#include "../defs.h"
#include "../lexer.h"

#include "rules.h"
#include <stdlib.h>

void skip(Parser* p) { p->token_idx++; }

AST* scan(Arena* context, Parser* p, TokenType t)
{
  AST* tree = ast_create(context, ASTType_TOKEN);
  tree->token_num = p->token_idx;
  if (next_token(p) == t) {
    skip(p);
    return tree;
  } else {
    switch(t)
    {
#define X(name, first, str) \
    case TokenType_##name: \
      die("missing token: %s\n", str);
    TokenTypeTable
#undef X
    }
  }
}

Parser* parser_create(Arena* context, Lexer* lexer)
{
  Parser *parser = arena_alloc(context, sizeof(Parser));
  parser->lexer = lexer;
  parser->token_idx = 0;
  parser->tree = r_PROGRAM(context, parser);
  return parser;
}

string* parser_to_string(Arena* arena, Parser* p)
{
  return ast_to_string(arena, p->tree, p->lexer, 0);
}
