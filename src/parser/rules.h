#include "ast.h"
#include "parser_internal.h"
#include "parser_shared.h"
#include <parser.h>
#include <stdarg.h>

#define X(name, str) \
  AST* r_##name(Arena*, Parser*);
  Rules
#undef X

// Helper Macros and Functions ==========================================

#define BRANCH_APPEND(tree, branch) ast_append(arena, tree, branch)
#define BRANCH_CREATE(type) ast_create(arena, ASTType_##type, parser->lexer)
#define CHECK_SCAN(token) ((NEXT_TOKEN == TokenType_##token) ? SCAN(token) : NULL)
#define END_RULE return tree; }
#define FIRST(n, ...) first(parser_next_token(parser)->type, n, __VA_ARGS__)
#define NEXT_TOKEN parser_next_token(parser)->type
#define RETURN_ROOT return tree
#define ROOT_APPEND(branch) ast_append(arena, tree, branch)
#define RULE(rule) r_##rule(arena, parser)
#define RULE_EXPR(name, opfun, next)\
  RULE_IMPL(name)\
    AST* op = NULL;\
    AST* n = RULE(next);\
    if ((op = opfun) != NULL) {\
      ROOT_APPEND(n);\
      do {\
        ROOT_APPEND(op);\
        ROOT_APPEND(RULE(next));\
      } while ((op = opfun) != NULL);\
    }\
    if (tree->num_children == 0) return n;\
  END_RULE
#define RULE_IMPL(name)\
  AST* r_##name(Arena* arena, Parser* parser) {\
    AST* tree = ast_create(arena, ASTType_##name, parser->lexer);
#define RULE_OP(name, n, ...)\
  RULE_IMPL(name)\
    return scan_op(arena, parser, n, __VA_ARGS__);\
  END_RULE
#define SCAN(token) scan(arena, parser, TokenType_##token)
#define SCAN_ADD(token) ast_append(arena, tree, scan(arena, parser, TokenType_##token))

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
  TokenType nt = parser_next_token(p)->type;
  va_list ptr;
  va_start(ptr, n);
  for(int i = 0; i < n; i++) {
    const TokenType check = va_arg(ptr, TokenType);
    if (nt == check) return scan(a, p, nt);
  }
  return NULL;
}

// Rule Implementations =================================================

RULE_IMPL(ASSIGN)
  AST* v, *r;
  if (NEXT_TOKEN == TokenType_ASSIGN) return SCAN(ASSIGN);
  else if (NEXT_TOKEN == TokenType_COLON_ASSIGN) return SCAN(COLON_ASSIGN);
  return NULL;
END_RULE

RULE_IMPL(BLOCK)
  SCAN(LBRACE);
  ROOT_APPEND(RULE(STMT_LIST));
  SCAN(RBRACE);
END_RULE

RULE_IMPL(BLOCK_STMT)
  if(NEXT_TOKEN == TokenType_IF) return RULE(BRANCH);
  else if(NEXT_TOKEN == TokenType_SWITCH) return RULE(SWITCH);
  else if(NEXT_TOKEN == TokenType_WHILE) return RULE(WHILE);
  return RULE(DO);
END_RULE

RULE_IMPL(BRANCH)
  SCAN(IF);
  ROOT_APPEND(RULE(EXPR));
  ROOT_APPEND(RULE(BLOCK));
  ROOT_APPEND(RULE(ELSE));
END_RULE

RULE_IMPL(CALL)
  SCAN(ID);
  ROOT_APPEND(RULE(TUPLE));
END_RULE

// RULE_IMPL(CASE)
//  if (FIRST(CASE))
// END_RULE

RULE_IMPL(DATA_TYPE)
  if (NEXT_TOKEN == TokenType_ID) SCAN_ADD(ID);
  else SCAN_ADD(NUMBER);
END_RULE

RULE_IMPL(DO)
  AST* b, *w;
  SCAN(DO);
  b = RULE(BLOCK);
  ROOT_APPEND(b);
  if (NEXT_TOKEN == TokenType_WHILE) {
    ROOT_APPEND(RULE(WHILE));
  }
END_RULE

RULE_IMPL(ELSE)
  AST *b;
  if(FIRST(1, TokenType_ELSE)) SCAN(ELSE);
  if(FIRST(1, TokenType_LBRACE)) {
    ROOT_APPEND(RULE(BLOCK));
  }
  else {
    b = RULE(BRANCH);
    ROOT_APPEND(b);
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

  if (FIRST(4, TokenType_IF, TokenType_FOR, TokenType_WHILE, TokenType_DO)) p = RULE(BLOCK_STMT);
  else p = RULE(EXPR_OR);
  if (t == TokenType_ID) {
    if ((a = RULE(ASSIGN)) != NULL) {
      ROOT_APPEND(a);
      ROOT_APPEND(p);
      ROOT_APPEND(RULE(EXPR));
      RETURN_ROOT;
    }
  }
  ROOT_APPEND(p);
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
    BRANCH_APPEND(op, RULE(PRIMARY));
    ROOT_APPEND(op);
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
  ROOT_APPEND(RULE(TUPLE));
  SCAN(BIGARROW);
  ROOT_APPEND(RULE(TUPLE));
END_RULE

RULE_IMPL(LITERAL)
  AST* n, *f;
  if (NEXT_TOKEN == TokenType_PERIOD) {
    f = RULE(FLOAT);
    ROOT_APPEND(f);
  } else if (NEXT_TOKEN == TokenType_NUMBER) {
    n = SCAN(NUMBER);
    if (NEXT_TOKEN == TokenType_PERIOD) {
      f = RULE(FLOAT);
      BRANCH_APPEND(f, n);
      ROOT_APPEND(f);
    } else ROOT_APPEND(n);
  } else if (NEXT_TOKEN == TokenType_HEX) SCAN_ADD(HEX);
  else if (NEXT_TOKEN == TokenType_OCTAL) SCAN_ADD(OCTAL);
  else SCAN_ADD(BINARY);
END_RULE

RULE_IMPL(METHOD_TYPE)
  ROOT_APPEND(RULE(TUPLE));
  SCAN(SMALLARROW);
  ROOT_APPEND(RULE(FUNCTION_TYPE));
END_RULE

RULE_IMPL(PROGRAM)
  ROOT_APPEND(RULE(STMT_LIST));
  // SCAN(EOF)
END_RULE

RULE_IMPL(PRIMARY)
  AST* r = NULL, *c;
  if (NEXT_TOKEN == TokenType_LPAR) r = RULE(TUPLE);
  else if (NEXT_TOKEN == TokenType_ID) r = RULE(VAR);
  if (NEXT_TOKEN == TokenType_LPAR) {
    c = BRANCH_CREATE(CALL);
    BRANCH_APPEND(c, r);
    BRANCH_APPEND(c, RULE(TUPLE));
    return c;
  } else if (CHECK_SCAN(SMALLARROW) != NULL){
    c = BRANCH_CREATE(METHOD);
    BRANCH_APPEND(c, r);
    BRANCH_APPEND(c, RULE(TUPLE));
    return c;
  }
  if (FIRST(5, TokenType_NUMBER, TokenType_HEX, TokenType_OCTAL, TokenType_BINARY, TokenType_PERIOD)) return RULE(LITERAL);
  return r;
END_RULE

RULE_IMPL(STMT)
  if(FIRST(3, TokenType_ID, TokenType_NUMBER, TokenType_STAR)) {
    ROOT_APPEND(RULE(EXPR));
    SCAN(SEMI);
  } else if (NEXT_TOKEN == TokenType_ENV) ROOT_APPEND(RULE(ENV));
  // TODO: handle CASE
  else ROOT_APPEND(RULE(BLOCK_STMT));
END_RULE

RULE_IMPL(STMT_LIST)
  if (FIRST(7,
        TokenType_ID, TokenType_NUMBER, TokenType_STAR, TokenType_IF,
        TokenType_SWITCH, TokenType_WHILE, TokenType_DO/*, TokenType_CASE */)) {
    ROOT_APPEND(RULE(STMT));
    ROOT_APPEND(RULE(STMT_LIST));
  }
END_RULE

RULE_IMPL(SWITCH)
  SCAN(SWITCH);
  ROOT_APPEND(RULE(EXPR));
  ROOT_APPEND(RULE(BLOCK));
END_RULE

RULE_IMPL(TAG)
  SCAN_ADD(ID);
  SCAN(COLON);
END_RULE

RULE_IMPL(TUPLE)
  if (NEXT_TOKEN == TokenType_LPAR)
  {
    SCAN(LPAR);
    ROOT_APPEND(RULE(TUPLE_BODY));
    SCAN(RPAR);
  }
  else if (NEXT_TOKEN == TokenType_ID) return RULE(VAR);
  else return RULE(LITERAL);
END_RULE

RULE_IMPL(TUPLE_BODY)
  ROOT_APPEND(RULE(EXPR));
  if (FIRST(1, TokenType_COMMA)) {
    SCAN(COMMA);
    ROOT_APPEND(RULE(TUPLE_BODY));
  }
END_RULE

RULE_IMPL(TUPLE_TYPE)
    SCAN(LPAR);
    ROOT_APPEND(RULE(TUPLE_TYPE_BODY));
    SCAN(RPAR);
END_RULE

RULE_IMPL(TUPLE_TYPE_BODY)
  ROOT_APPEND(RULE(TUPLE_TYPE_BODY_TYPE));
  if (FIRST(1, TokenType_COMMA)) {
    SCAN(COMMA);
    ROOT_APPEND(RULE(TUPLE_TYPE_BODY));
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
      t1 = BRANCH_CREATE(ASSIGN);
      BRANCH_APPEND(t1, v);
      BRANCH_APPEND(t1, e);
      ROOT_APPEND(t1);
      RETURN_ROOT;
    } else if (FIRST(1, TokenType_COLON_ASSIGN)) {
      SCAN(COLON_ASSIGN);
      e = RULE(EXPR);
      t1 = BRANCH_CREATE(COLON_ASSIGN);
      BRANCH_APPEND(t1, v);
      BRANCH_APPEND(t1, e);
      ROOT_APPEND(t1);
      RETURN_ROOT;
    }
    ROOT_APPEND(v);
  } else {
    ROOT_APPEND(RULE(TYPE));
  }
END_RULE

RULE_IMPL(TYPE)
  AST *m, *s;

  // Handle simple types
  if(FIRST(1, TokenType_ID)) SCAN_ADD(ID);
  else if(FIRST(1, TokenType_NUMBER)) SCAN_ADD(NUMBER);
  else if(FIRST(1, TokenType_LPAR)) {
    ROOT_APPEND(RULE(TUPLE_TYPE));
  }
  else if (FIRST(1, TokenType_STAR)) {
    s = SCAN(STAR);
    BRANCH_APPEND(m, RULE(TYPE));
  }
  else if (FIRST(1, TokenType_MUT)) {
    m = SCAN(MUT);
    BRANCH_APPEND(m, RULE(TYPE));
  }

  // Handle methods and functions
  if (FIRST(2, TokenType_BIGARROW, TokenType_SMALLARROW)) {
    if (NEXT_TOKEN == TokenType_SMALLARROW) {
      SCAN(SMALLARROW);
      ROOT_APPEND(RULE(TUPLE_TYPE));
    }
    SCAN(BIGARROW);
    ROOT_APPEND(RULE(TUPLE_TYPE));
  }
END_RULE

RULE_IMPL(UNARY)
  AST* r;
  if (NEXT_TOKEN == TokenType_STAR) {
    r = SCAN(STAR);
    BRANCH_APPEND(r, RULE(TUPLE));
  } else return RULE(TUPLE);
  return r;
END_RULE

// pointer decl:  *var
// pointer deref: *var or var*?
RULE_IMPL(VAR)
  if (NEXT_TOKEN == TokenType_ID) SCAN_ADD(ID);
  else if (NEXT_TOKEN == TokenType_STAR) ROOT_APPEND(RULE(VAR));
  if (NEXT_TOKEN == TokenType_COLON) {
    SCAN(COLON);
    ROOT_APPEND(RULE(TYPE));
  } else if(NEXT_TOKEN == TokenType_PERIOD) {
    SCAN(PERIOD);
    ROOT_APPEND(RULE(VAR));
  }
END_RULE

RULE_IMPL(WHILE)
  SCAN(WHILE);
  ROOT_APPEND(RULE(EXPR));
  ROOT_APPEND(RULE(BLOCK));
END_RULE

#undef X
