#include "ast.h"
#include "parser_internal.h"
#include <parser.h>
#include <defs.h>
#include <lexer.h>

#include "rules.h"

void skip(Parser* p) { parser_next_token(p); }

AST* scan(Arena* context, Parser* p, TokenType t)
{
  AST* tree = ast_create(context, ASTType_TOKEN, p->lexer);
  tree->token_line = p->line_no;
  tree->token_idx = p->token_idx;

  TokenType nt = parser_next_token(p)->type;
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
  parser->context = context;
  parser->lexer = lexer;
  parser->token_idx = 0;
  parser->line_no = 0;
  return parser;
}

const Token* parser_next_token(Parser* p)
{
  if (p->token_idx >= lexer_get_line_len(p->lexer, p->line_no)) {
    p->line_no++;
    p->token_idx = 0;
  }
  return lexer_get_token(p->lexer, p->line_no, p->token_idx++);
}

AST* parser_generate_ast(Parser* p)
{
  return r_PROGRAM(p->context, p);
}
