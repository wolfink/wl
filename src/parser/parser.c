#include "ast.h"
#include "parser_internal.h"
#include <parser.h>
#include <defs.h>
#include <lexer.h>

#include "rules.h"

void skip(Parser* p) { p->token_idx++; }

AST* scan(Arena* context, Parser* p, TokenType t)
{
  AST* tree = ast_create(context, ASTType_TOKEN, p->lexer);
  tree->token_num = p->token_idx;

  TokenType nt = next_token(p);
  if (nt == t) {
    skip(p);
    return tree;
  } else {
    char* nts, *ts;
    switch(t)
    {
#define X(name, first, str) \
    case TokenType_##name: \
      ts = str; \
      break;
    TokenTypeTable
#undef X
    default:
      ts = "";
    }
    switch(nt)
    {
#define X(name, first, str) \
    case TokenType_##name: \
      nts = str; \
      break;
    TokenTypeTable
#undef X
    default:
      nts = "";
    }
    die("expected: %s, got: %s\n", ts, nts);
  }
  return NULL;
}

Parser* parser_create(Arena* context, Lexer* lexer)
{
  Parser *parser = arena_alloc(context, sizeof(Parser));
  parser->lexer = lexer;
  parser->token_idx = 0;
  parser->tree = r_PROGRAM(context, parser);
  return parser;
}

AST* parser_get_ast(Parser* p)
{
  return p->tree;
}

string* parser_to_string(Arena* arena, Parser* p)
{
  return ast_to_string(arena, p->tree, 0);
}
