#define PARSER_INTERNAL
#define AST_INTERNAL

#include <defs.h>

#include "rules.h"

void skip(Parser* p) { p->token_idx++; }


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
  size_t line_len = lexer_get_line_len(p->lexer, p->line_no);
  while (p->token_idx >= line_len && p->line_no < lexer_get_num_lines(p->lexer) - 1) {
    p->line_no++;
    p->token_idx -= line_len;
    line_len = lexer_get_line_len(p->lexer, p->line_no);
  }
  return lexer_get_token(p->lexer, p->line_no, p->token_idx);
}

AST* parser_generate_ast(Parser* p)
{
  return r_PROGRAM(p->context, p);
}

AST* scan(Arena* context, Parser* p, TokenType t)
{
  AST* tree = ast_create(context, ASTType_TOKEN, p->lexer);
  tree->token_line = p->line_no;
  tree->token_idx = p->token_idx;

  const Token* nt   = parser_next_token(p);
  TokenType nt_type = nt->type;
  if (nt_type == t) {
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
    switch(nt_type)
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
    die("%s:%lu:%lu: " ANSI_COLOR_RED "error: " ANSI_COLOR_RESET "expected: %s, got: %s\n",
      lexer_get_filename(p->lexer),
      p->line_no + 1,
      nt->start,
      ts,
      nts);
  }
  return NULL;
}
