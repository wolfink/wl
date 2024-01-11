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
#define NEXT_TOKEN next_token(parser)
#define SCAN(token) scan(arena, parser, TokenType_##token)
#define SCAN_ADD(token) ast_append(arena, tree, scan(arena, parser, TokenType_##token))
#define CHECK_SCAN(token) ((NEXT_TOKEN == TokenType_##token) ? SCAN(token) : NULL)
#define APPEND_TREE(branch) ast_append(arena, tree, branch)
#define APPEND_AST(tree, branch) ast_append(arena, tree, branch)
#define RETURN return tree
//#define X(cmp) NEXT_TOKEN == TokenType_##cmp ||
#define FIRST(n, ...) first(next_token(parser), n, __VA_ARGS__)
#define END_RULE return tree; }
#define RULE_EXPR(name, opfun, next) \
RULE_IMPL(name) \
  AST* op = NULL; \
  AST* n = RULE(next); \
  if ((op = opfun) != NULL) { \
    APPEND_TREE(n); \
    do { \
      APPEND_TREE(op); \
      APPEND_TREE(RULE(next)); \
    } while ((op = opfun) != NULL); \
  } \
  if (tree->num_children == 0) return n; \
END_RULE

#define RULE_OP(name, n, ...) \
RULE_IMPL(name) \
  return scan_op(arena, parser, n, __VA_ARGS__); \
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
  return NULL;
}


RULE_IMPL(ASSIGN)
  AST* v, *r;
  if (NEXT_TOKEN == TokenType_ASSIGN) return SCAN(ASSIGN);
  else if (NEXT_TOKEN == TokenType_COLON_ASSIGN) return SCAN(COLON_ASSIGN);
  return NULL;
END_RULE

RULE_IMPL(BLOCK)
  SCAN(LBRACE);
  APPEND_TREE(RULE(STMT_LIST));
  SCAN(RBRACE);
END_RULE

RULE_IMPL(BRANCH)
  SCAN(IF);
  APPEND_TREE(RULE(EXPR));
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
  AST* a = NULL, *p = NULL;
  TokenType t = NEXT_TOKEN;

  p = RULE(EXPR_OR);
  if (t == TokenType_ID) {
    if ((a = RULE(ASSIGN)) != NULL) {
      APPEND_TREE(a);
      APPEND_TREE(p);
      APPEND_TREE(RULE(EXPR));
      RETURN;
    }
  } 
  APPEND_TREE(p);
END_RULE

RULE_EXPR(EXPR_OR, CHECK_SCAN(OR), EXPR_AND)
RULE_EXPR(EXPR_AND, CHECK_SCAN(AND), EXPR_BW_OR)
RULE_EXPR(EXPR_BW_OR, CHECK_SCAN(BW_OR), EXPR_BW_AND)
RULE_EXPR(EXPR_BW_AND, CHECK_SCAN(BW_AND), EXPR_EQ)
RULE_EXPR(EXPR_EQ, RULE(OP_EQ), EXPR_REL)
RULE_EXPR(EXPR_REL, RULE(OP_REL), EXPR_SUM);
RULE_EXPR(EXPR_SUM, RULE(OP_SUM), EXPR_MUL);
RULE_EXPR(EXPR_MUL, RULE(OP_MUL), EXPR_UNARY);
RULE_IMPL(EXPR_UNARY)
  AST* op = RULE(OP_UNARY);
  if (op != NULL) while (op != NULL) {
    APPEND_AST(op, RULE(PRIMARY));
    APPEND_TREE(op);
    op = RULE(OP_UNARY);
  } else return RULE(PRIMARY);
END_RULE

RULE_OP(OP_EQ, 1, TokenType_EQUALS);
RULE_OP(OP_REL, 2, TokenType_LANGLE, TokenType_RANGLE);
RULE_OP(OP_SUM, 2, TokenType_PLUS, TokenType_MINUS);
RULE_OP(OP_MUL, 2, TokenType_STAR, TokenType_FSLASH);
RULE_OP(OP_UNARY, 1, TokenType_STAR);

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
    if (NEXT_TOKEN == TokenType_PERIOD) {
      f = RULE(FLOAT);
      APPEND_AST(f, n);
      APPEND_TREE(f);
    } else APPEND_TREE(n);
  } else if (NEXT_TOKEN == TokenType_HEX) SCAN_ADD(HEX);
  else if (NEXT_TOKEN == TokenType_OCTAL) SCAN_ADD(OCTAL);
  else SCAN_ADD(BINARY);
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

RULE_IMPL(PRIMARY)
  AST* r = NULL, *c;
  if (NEXT_TOKEN == TokenType_LPAR) r = RULE(TUPLE);
  else if (NEXT_TOKEN == TokenType_ID) r = RULE(VAR);
  if (NEXT_TOKEN == TokenType_LPAR) {
    c = ast_create(arena, ASTType_CALL);
    APPEND_AST(c, r);
    APPEND_AST(c, RULE(TUPLE));
    return c;
  } else if (CHECK_SCAN(SMALLARROW) != NULL){
    c = ast_create(arena, ASTType_METHOD);
    APPEND_AST(c, r);
    APPEND_AST(c, RULE(TUPLE));
    return c;
  }
  if (FIRST(4, TokenType_NUMBER, TokenType_HEX, TokenType_OCTAL, TokenType_BINARY)) return RULE(LITERAL);
  return r;
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

RULE_IMPL(TUPLE)
  if (NEXT_TOKEN == TokenType_LPAR)
  {
    SCAN(LPAR);
    APPEND_TREE(RULE(TUPLE_BODY));
    SCAN(RPAR);
  }
  else if (NEXT_TOKEN == TokenType_ID) return RULE(VAR);
  else return RULE(LITERAL);
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
      APPEND_AST(t1, v);
      APPEND_AST(t1, e);
      APPEND_TREE(t1);
      RETURN;
    } else if (FIRST(1, TokenType_COLON_ASSIGN)) {
      SCAN(COLON_ASSIGN);
      e = RULE(EXPR);
      t1 = ast_create(arena, ASTType_COLON_ASSIGN);
      APPEND_AST(t1, v);
      APPEND_AST(t1, e);
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
    APPEND_AST(m, RULE(TYPE));
  }
  else if (FIRST(1, TokenType_MUT)) {
    m = SCAN(MUT);
    APPEND_AST(m, RULE(TYPE));
  }

  // Handle methods and functions
  if (FIRST(2, TokenType_BIGARROW, TokenType_SMALLARROW)) {
    if (NEXT_TOKEN == TokenType_SMALLARROW) {
      SCAN(SMALLARROW);
      APPEND_TREE(RULE(TUPLE_TYPE));
    }
    SCAN(BIGARROW);
    APPEND_TREE(RULE(TUPLE_TYPE));
  }
END_RULE

RULE_IMPL(UNARY)
  AST* r;
  if (NEXT_TOKEN == TokenType_STAR) {
    r = SCAN(STAR);
    APPEND_AST(r, RULE(TUPLE));
  } else return RULE(TUPLE);
  return r;
END_RULE

// pointer decl:  *var
// pointer deref: *var or var*?
RULE_IMPL(VAR)
  if (NEXT_TOKEN == TokenType_ID) SCAN_ADD(ID);
  else if (NEXT_TOKEN == TokenType_STAR) APPEND_TREE(RULE(VAR));
  if (NEXT_TOKEN == TokenType_COLON) {
    SCAN(COLON);
    APPEND_TREE(RULE(TYPE));
  } else if(NEXT_TOKEN == TokenType_PERIOD) {
    SCAN(PERIOD);
    APPEND_TREE(RULE(VAR));
  }
END_RULE

RULE_IMPL(WHILE)
  SCAN(WHILE);
  APPEND_TREE(RULE(EXPR));
  APPEND_TREE(RULE(BLOCK));
END_RULE

#undef X
