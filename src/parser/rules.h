#include "ast.h"
#include "parser_internal.h"
#include "parser_shared.h"
#include <stdarg.h>

#define X(name, str) \
  AST* r_##name(Arena*, Parser*);
  Rules
#undef X

#define RULE(rule) r_##rule(arena, parser)
#define RULE_IMPL(name) \
AST* r_##name(Arena* arena, Parser* parser) { \
  AST* tree = ast_create(arena, ASTType_##name);
#define SCAN(token) scan(arena, parser, TokenType_##token)
#define SCAN_ADD(token) ast_append(arena, tree, scan(arena, parser, TokenType_##token))
#define APPEND_TREE(branch) ast_append(arena, tree, branch)
#define RETURN return tree
#define NEXT_TOKEN next_token(parser)
//#define X(cmp) NEXT_TOKEN == TokenType_##cmp ||
#define FIRST(n, ...) first(next_token(parser), n, __VA_ARGS__)
#define END_RULE return tree; }
#define RULE_TERM(val, valm1, n, ...) \
RULE_IMPL(TERM##val) \
  AST* t1 = &zero, *t2 = &zero, *op = &zero; \
  if (FIRST(2, TokenType_ID, TokenType_STAR)) t1 = RULE(VAR); \
  else if (FIRST(1, TokenType_NUMBER)) t1 = RULE(LITERAL); \
  while(FIRST(n, __VA_ARGS__)) { \
    op = scan_op(arena, parser, n, __VA_ARGS__); \
    t2 = RULE(TERM##valm1); \
    if (t1->num_children != 0) ast_append(arena, op, t1); \
    ast_append(arena, op, t2); \
    APPEND_TREE(op); \
  } \
  if (tree->num_children == 0) { \
    t2 = RULE(TERM##valm1); \
    if (t2->num_children == 0) return t1; \
    if (t1->num_children != 0) ast_append(arena, t2, t1); \
    return t2; \
  } \
END_RULE

int first(TokenType t, size_t n, ...)
{
  va_list ptr;
  va_start(ptr, n);
  for(int i = 0; i < n; i++)
    if (t == va_arg(ptr, TokenType)) return 1;
  va_end(ptr);
  return 0;
}

AST* scan_op(Arena* a, Parser* p, size_t n, ...)
{
  TokenType nt = next_token(p);
  va_list ptr;
  va_start(ptr, n);
  for(int i = 0; i < n; i++) {
    const TokenType check = va_arg(ptr, TokenType);
    if (nt == check) return scan(a, p, nt);
  }
  switch (nt) {
#define X(name, str)\
  case ASTType_##name: \
    die("scan_op: unexpected token: %s", str); break;
  Rules
#undef X
  }
  return NULL;
}


// RULE_IMPL(ASSIGN)
//   RULE(VAR);
//   SCAN(ASSIGN);
//   RULE(EXPR)
// END_RULE

RULE_IMPL(BLOCK)
  SCAN(LBRACE);
  APPEND_TREE(RULE(STMT_LIST));
  SCAN(RBRACE);
END_RULE

RULE_IMPL(BRANCH)
  SCAN(IF);
  APPEND_TREE(RULE(TERM1));
  APPEND_TREE(RULE(BLOCK));
  APPEND_TREE(RULE(ELSE));
END_RULE

RULE_IMPL(CALL)
  SCAN(ID);
  APPEND_TREE(RULE(TUPLE));
END_RULE

// RULE_IMPL(CASE)
//  if (FIRST(CASE))
// END_RULE

RULE_IMPL(DATA_TYPE)
  if (FIRST(1, TokenType_ID)) SCAN_ADD(ID);
  else SCAN_ADD(NUMBER);
END_RULE

RULE_IMPL(DO)
  AST* b, *w;
  SCAN(DO);
  b = RULE(BLOCK);
  APPEND_TREE(b);
  if (FIRST(1, TokenType_WHILE)) {
    APPEND_TREE(RULE(WHILE));
  }
END_RULE

RULE_IMPL(ELSE)
  AST *b;
  if(FIRST(1, TokenType_ELSE)) SCAN(ELSE);
  if(FIRST(1, TokenType_LBRACE)) {
    APPEND_TREE(RULE(BLOCK));
  }
  else {
    b = RULE(BRANCH);
    APPEND_TREE(b);
  }
END_RULE

RULE_IMPL(ENV)
  if (FIRST(1, TokenType_PLUS)) SCAN_ADD(PLUS);
  else if (FIRST(1, TokenType_MINUS)) SCAN_ADD(MINUS);
  SCAN_ADD(ID);
END_RULE

RULE_IMPL(EXPR)
  APPEND_TREE(RULE(TERM9));
  RETURN;
  /*
  AST* n, *v, *e, *t1, *t2, *op;
  if (FIRST(1, TokenType_NUMBER)) {
    n = RULE(LITERAL);
    APPEND_TREE(n);
  }
  else if (FIRST(2, TokenType_ID, TokenType_STAR)) {
    v = RULE(VAR);
    while (FIRST(2, TokenType_ASSIGN, TokenType_COLON_ASSIGN)) {
      op = scan_op(arena, parser, 2, TokenType_ASSIGN, TokenType_COLON_ASSIGN);
      t2 = RULE(TERM9);
      ast_append(arena, op, v);
      ast_append(arena, op, t2);
      APPEND_TREE(op);
    }
    if (tree->num_children == 0) APPEND_TREE(v);
  }
  */
END_RULE

RULE_IMPL(FACTOR)
  AST* v;
  if (FIRST(2, TokenType_ID, TokenType_STAR)) {
    v = RULE(VAR);
    APPEND_TREE(v);
  }
  else APPEND_TREE(RULE(LITERAL));
END_RULE

RULE_IMPL(FLOAT)
  SCAN(PERIOD);
  SCAN_ADD(NUMBER);
END_RULE

RULE_IMPL(FUNCTION_TYPE)
  APPEND_TREE(RULE(TUPLE));
  SCAN(BIGARROW);
  APPEND_TREE(RULE(TUPLE));
END_RULE

RULE_IMPL(LITERAL)
  AST* n, *f;
  if (FIRST(1, TokenType_NUMBER)) {
    n = SCAN(NUMBER);
    if (FIRST(1, TokenType_PERIOD)) {
      f = RULE(FLOAT);
      ast_append(arena, f, n);
      APPEND_TREE(f);
    } else APPEND_TREE(n);
  }
END_RULE

RULE_IMPL(METHOD_TYPE)
  APPEND_TREE(RULE(TUPLE));
  SCAN(SMALLARROW);
  APPEND_TREE(RULE(FUNCTION_TYPE));
END_RULE

RULE_IMPL(PROGRAM)
  APPEND_TREE(RULE(STMT_LIST));
  // SCAN(EOF)
END_RULE

RULE_IMPL(STMT)
  if(FIRST(3, TokenType_ID, TokenType_NUMBER, TokenType_STAR)) {
    APPEND_TREE(RULE(EXPR));
    SCAN(SEMI);
  }
  else if(FIRST(1, TokenType_IF)) APPEND_TREE(RULE(BRANCH));
  else if(FIRST(1, TokenType_SWITCH)) {
    APPEND_TREE(RULE(SWITCH));
    SCAN(SEMI);
  }
  else if(FIRST(1, TokenType_WHILE)) APPEND_TREE(RULE(WHILE));
  else if(FIRST(1, TokenType_DO)) APPEND_TREE(RULE(DO));
  // TODO: handle CASE
  else APPEND_TREE(RULE(ENV));
END_RULE

RULE_IMPL(STMT_LIST)
  if (FIRST(7,
        TokenType_ID, TokenType_NUMBER, TokenType_STAR, TokenType_IF,
        TokenType_SWITCH, TokenType_WHILE, TokenType_DO/*, TokenType_CASE */)) {
    APPEND_TREE(RULE(STMT));
    APPEND_TREE(RULE(STMT_LIST));
  }
END_RULE

RULE_IMPL(SWITCH)
  SCAN(SWITCH);
  APPEND_TREE(RULE(EXPR));
  APPEND_TREE(RULE(BLOCK));
END_RULE

RULE_IMPL(TAG)
  SCAN_ADD(ID);
  SCAN(COLON);
END_RULE

RULE_TERM(9, 8, 2, TokenType_ASSIGN, TokenType_COLON_ASSIGN)
RULE_TERM(8, 7, 1, TokenType_OR)
RULE_TERM(7, 6, 1, TokenType_AND)
RULE_TERM(6, 4, 1, TokenType_BW_OR) // Skip 5 for now
RULE_TERM(4, 3, 1, TokenType_BW_AND)
RULE_TERM(3, 2, 1, TokenType_EQUALS)
RULE_TERM(2, 1, 2, TokenType_LANGLE, TokenType_RANGLE)
//RULE_TERM(1, 0, 2, TokenType_PLUS, TokenType_MINUS)

AST *r_TERM1(Arena *arena, Parser *parser) {
  AST *tree = ast_create(arena, ASTType_TERM1);
  AST *t1 = &zero, *t2 = &zero, *op = &zero;
  if (first(next_token(parser), 2, TokenType_ID, TokenType_STAR))
    t1 = r_VAR(arena, parser);
  else if (first(next_token(parser), 1, TokenType_NUMBER))
    t1 = r_LITERAL(arena, parser);
  while (first(next_token(parser), 2, TokenType_PLUS, TokenType_MINUS)) {
    op = scan_op(arena, parser, 2, TokenType_PLUS, TokenType_MINUS);
    t2 = r_TERM0(arena, parser);
    if (t1->num_children != 0)
      ast_append(arena, op, t1);
    ast_append(arena, op, t2);
    ast_append(arena, tree, op);
  }
  if (tree->num_children == 0) {
    t2 = r_TERM0(arena, parser);
    if (t2->num_children == 0)
      return t1;
    if (t1->num_children != 0)
      ast_append(arena, t2, t1);
    return t2;
  }
  return tree;
}

RULE_IMPL(TERM0)
  AST* t1 = &zero, *f = &zero;
  if (FIRST(2, TokenType_ID, TokenType_STAR)) t1 = RULE(VAR);
  else if (FIRST(1, TokenType_NUMBER)) t1 = RULE(LITERAL);
  while(FIRST(2, TokenType_STAR, TokenType_FSLASH)) {
    AST* op;
    if (FIRST(1, TokenType_STAR)) op = SCAN(STAR);
    else op = SCAN(FSLASH);
    f = RULE(FACTOR);
    ast_append(arena, op, f);
    APPEND_TREE(op);
  }
  if (tree->num_children == 0) return t1;
END_RULE

RULE_IMPL(TUPLE)
  SCAN(LPAR);
  APPEND_TREE(RULE(TUPLE_BODY));
  SCAN(RPAR);
END_RULE

RULE_IMPL(TUPLE_BODY)
  APPEND_TREE(RULE(EXPR));
  if (FIRST(1, TokenType_COMMA)) {
    SCAN(COMMA);
    APPEND_TREE(RULE(TUPLE_BODY));
  }
END_RULE

RULE_IMPL(TUPLE_TYPE)
    SCAN(LPAR);
    APPEND_TREE(RULE(TUPLE_TYPE_BODY));
    SCAN(RPAR);
END_RULE

RULE_IMPL(TUPLE_TYPE_BODY)
  APPEND_TREE(RULE(TUPLE_TYPE_BODY_TYPE));
  if (FIRST(1, TokenType_COMMA)) {
    SCAN(COMMA);
    APPEND_TREE(RULE(TUPLE_TYPE_BODY));
  }
END_RULE

RULE_IMPL(TUPLE_TYPE_BODY_TYPE)
  AST* v, *e, *t1;
  if (FIRST(1, TokenType_ID)) {
    // Copied from EXPR
    // TODO: refactor into separate function
    v = RULE(VAR);
    if (FIRST(1, TokenType_ASSIGN)) {
      SCAN(ASSIGN);
      e = RULE(EXPR);
      t1 = ast_create(arena, ASTType_ASSIGN);
      ast_append(arena, t1, v);
      ast_append(arena, t1, e);
      APPEND_TREE(t1);
      RETURN;
    } else if (FIRST(1, TokenType_COLON_ASSIGN)) {
      SCAN(COLON_ASSIGN);
      e = RULE(EXPR);
      t1 = ast_create(arena, ASTType_COLON_ASSIGN);
      ast_append(arena, t1, v);
      ast_append(arena, t1, e);
      APPEND_TREE(t1);
      RETURN;
    }
    APPEND_TREE(v);
  } else {
    APPEND_TREE(RULE(TYPE));
  }
END_RULE

RULE_IMPL(TYPE)
  AST *m, *s;

  // Handle simple types
  if(FIRST(1, TokenType_ID)) SCAN_ADD(ID);
  else if(FIRST(1, TokenType_NUMBER)) SCAN_ADD(NUMBER);
  else if(FIRST(1, TokenType_LPAR)) {
    APPEND_TREE(RULE(TUPLE_TYPE));
  }
  else if (FIRST(1, TokenType_STAR)) {
    s = SCAN(STAR);
    ast_append(arena, m, RULE(TYPE));
  }
  else if (FIRST(1, TokenType_MUT)) {
    m = SCAN(MUT);
    ast_append(arena, m, RULE(TYPE));
  }

  // Handle methods and functions
  if (FIRST(2, TokenType_BIGARROW, TokenType_SMALLARROW)) {
    if (FIRST(1, TokenType_SMALLARROW)) {
      SCAN(SMALLARROW);
      APPEND_TREE(RULE(TUPLE_TYPE));
    }
    SCAN(BIGARROW);
    APPEND_TREE(RULE(TUPLE_TYPE));
  }
END_RULE

RULE_IMPL(WHILE)
  SCAN(WHILE);
  APPEND_TREE(RULE(EXPR));
  APPEND_TREE(RULE(BLOCK));
END_RULE

// pointer decl:  *var
// pointer deref: *var or var*?
RULE_IMPL(VAR)
  if (FIRST(1, TokenType_ID)) SCAN_ADD(ID);
  else if (FIRST(1, TokenType_STAR)) APPEND_TREE(RULE(VAR));
  if (FIRST(1, TokenType_COLON)) {
    SCAN(COLON);
    APPEND_TREE(RULE(TYPE));
  } else if(FIRST(1, TokenType_PERIOD)) {
    SCAN(PERIOD);
    APPEND_TREE(RULE(VAR));
  }
END_RULE

#undef X
