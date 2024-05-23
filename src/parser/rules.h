#define PARSER_INTERNAL
#define AST_INTERNAL

#include <ast.h>
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
  switch(NEXT_TOKEN) {
    case TokenType_ASSIGN: return SCAN(ASSIGN);
    case TokenType_COLON_ASSIGN: return SCAN(COLON_ASSIGN);
    case TokenType_PLUS_EQUALS: return SCAN(PLUS_EQUALS);
    case TokenType_MINUS_EQUALS: return SCAN(MINUS_EQUALS);
    case TokenType_AND_EQUALS: return SCAN(AND_EQUALS);
    case TokenType_STAR_EQUALS: return SCAN(STAR_EQUALS);
    case TokenType_SLASH_EQUALS: return SCAN(SLASH_EQUALS);
    default: return NULL;
  }
END_RULE

RULE_IMPL(BLOCK)
  SCAN(LBRACE);
  ROOT_APPEND(RULE(STMT_LIST));
  SCAN(RBRACE);
END_RULE

// BLOCK_STMT: <BRANCH> | <SWITCH> | <WHILE> | <DO>
// FIRST(<BRANCH>) = IF
// FIRST(<SWITCH>) = SWITCH
// FIRST(<WHILE>) = WHILE
// FIRST(<DO>) = DO
RULE_IMPL(BLOCK_STMT)
  if(NEXT_TOKEN == TokenType_IF) return RULE(BRANCH);
  else if(NEXT_TOKEN == TokenType_SWITCH) return RULE(SWITCH);
  else if(NEXT_TOKEN == TokenType_WHILE) return RULE(WHILE);
  return RULE(DO);
END_RULE

// BRANCH: IF <EXPR> <BLOCK> <ELSE>
RULE_IMPL(BRANCH)
  SCAN(IF);
  ROOT_APPEND(RULE(EXPR));
  ROOT_APPEND(RULE(BLOCK));
  ROOT_APPEND(RULE(ELSE));
END_RULE

// CALL: ID <TUPLE>
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

// DO: DO <BLOCK> | DO <BLOCK> WHILE <WHILE>
RULE_IMPL(DO)
  AST* b, *w;
  SCAN(DO);
  ROOT_APPEND(RULE(BLOCK));
  if (NEXT_TOKEN == TokenType_WHILE) {
    ROOT_APPEND(RULE(WHILE));
  }
END_RULE

// ELSE: ELSE <BLOCK> | ELSE <BRANCH>
// FIRST(<BLOCK>) = LBRACE
RULE_IMPL(ELSE)
  SCAN(ELSE);
  if(NEXT_TOKEN == TokenType_LBRACE) ROOT_APPEND(RULE(BLOCK));
  else ROOT_APPEND(RULE(BRANCH));
END_RULE

// ENV: (PLUS | MINUS| NULL) ID
RULE_IMPL(ENV)
  if (NEXT_TOKEN == TokenType_PLUS) SCAN_ADD(PLUS);
  else if (NEXT_TOKEN == TokenType_MINUS) SCAN_ADD(MINUS);
  SCAN_ADD(ID);
END_RULE

// EXPR: <BLOCK_STMT> | <LOAD> | <EXPR_OR> | ID <ASSIGN> <EXPR> | ID MACRO
// FIRST(<BLOCK_STMT>) = IF | FOR | WHILE | DO
// FIRST(<LOAD>) = LOAD
// FIRST(<EXPR_OR>) = ID | <EXPR_UNARY>
// FOLLOW(ID) = EQUALS | PLUS_EQUALS | MINUS_EQUALS | ... etc. | MACRO
RULE_IMPL(EXPR)
  AST* a = NULL, *p = NULL;
  TokenType t = NEXT_TOKEN;

  if (FIRST(4, TokenType_IF, TokenType_FOR, TokenType_WHILE, TokenType_DO)) p = RULE(BLOCK_STMT);
  else if (t == TokenType_LOAD) p = RULE(LOAD);
  else if (t == TokenType_SIZEOF) p = RULE(SIZEOF);
  else p = RULE(EXPR_OR);
  if (t == TokenType_ID) {
    while (NEXT_TOKEN == TokenType_LBRACKET) {
      AST* l = SCAN(LBRACKET);
      BRANCH_APPEND(p, l);
      BRANCH_APPEND(p, RULE(EXPR));
      SCAN(RBRACKET);
    }
    if ((a = RULE(ASSIGN)) != NULL) {
      ROOT_APPEND(a);
      ROOT_APPEND(p);
      if (NEXT_TOKEN == TokenType_LBRACE) ROOT_APPEND(RULE(BLOCK_STMT));
      else ROOT_APPEND(RULE(EXPR));
      RETURN_ROOT;
    }
    if (NEXT_TOKEN == TokenType_MACRO) SCAN_ADD(MACRO);
  }
  ROOT_APPEND(p);
END_RULE

// Operator precendence

// RULE_EXPR: <LOWER_EXPR> OP <RULE_EXPR>
// FIRST(OP) = TT1 | TT2 | .. | TTN
// FOLLOW(LOWER_EXPR) = OP
RULE_EXPR(EXPR_OR, CHECK_SCAN(OR), EXPR_AND)
RULE_EXPR(EXPR_AND, CHECK_SCAN(AND), EXPR_BW_OR)
RULE_EXPR(EXPR_BW_OR, CHECK_SCAN(BW_OR), EXPR_BW_AND)
RULE_EXPR(EXPR_BW_AND, CHECK_SCAN(BW_AND), EXPR_EQ)
RULE_EXPR(EXPR_EQ, RULE(OP_EQ), EXPR_REL)
RULE_EXPR(EXPR_REL, RULE(OP_REL), EXPR_SUM);
RULE_EXPR(EXPR_SUM, RULE(OP_SUM), EXPR_MUL);
RULE_EXPR(EXPR_MUL, RULE(OP_MUL), EXPR_UNARY);

// EXPR_UNARY: <PREFIX> <PRIMARY> <POSTFIX>
// PREFIX: TILDE <PREFIX> | NULL
// POSTFIX: COMMA <RANGE> | TILDE <POSTFIX> | PERIOD ID
// FIRST(<PREFIX>) = TILDE
// FIRST(<PRIMARY>) = ID | LPAR
// FIRST(<POSTFIX>) = DBL_PERIOD | TILDE
// FOLLOW(<PREFIX>) = ID | LPAR
// FOLLOW(<PRIMARY>) = DBL_PERIOD | TILDE
RULE_IMPL(EXPR_UNARY)
  AST* pre = NULL, *val = NULL, *post = NULL;
  // <PREFIX>
  while (NEXT_TOKEN == TokenType_TILDE) {
    pre = SCAN(TILDE);
  }
  // <PRIMARY>
  val = RULE(PRIMARY);
  if (pre != NULL) {
    BRANCH_APPEND(pre, val);
  }
  // <POSTFIX>
  if (FIRST(3, TokenType_DBL_PERIOD, TokenType_PERIOD, TokenType_TILDE)) {
    while (FIRST(3, TokenType_DBL_PERIOD, TokenType_PERIOD, TokenType_TILDE)) {
      if (NEXT_TOKEN == TokenType_DBL_PERIOD) {
        post = RULE(RANGE);
        if (pre != NULL) BRANCH_APPEND(post, pre);
        else BRANCH_APPEND(post, val);
        ROOT_APPEND(post);
        RETURN_ROOT;
      } else if (NEXT_TOKEN == TokenType_PERIOD) {
        post = SCAN(PERIOD);
        if (pre != NULL) BRANCH_APPEND(post, pre);
        else BRANCH_APPEND(post, val);
        BRANCH_APPEND(post, SCAN(ID));
        ROOT_APPEND(post);
        continue;
      }
      post = SCAN(TILDE);
      if (pre != NULL) BRANCH_APPEND(post, pre);
      else BRANCH_APPEND(post, val);
      ROOT_APPEND(post);
    }
  }
  else return val;
  
  // if (op != NULL) while (op != NULL) {
  //   BRANCH_APPEND(op, RULE(PRIMARY));
  //   ROOT_APPEND(op);
  //   op = RULE(OP_UNARY);
  // } else return RULE(PRIMARY);
END_RULE

RULE_OP(OP_EQ, 4, TokenType_EQUALS, TokenType_NOT_EQUALS, TokenType_GT_EQUALS, TokenType_LT_EQUALS);
RULE_OP(OP_REL, 2, TokenType_LANGLE, TokenType_RANGLE);
RULE_OP(OP_SUM, 2, TokenType_PLUS, TokenType_MINUS);
RULE_OP(OP_MUL, 2, TokenType_STAR, TokenType_FSLASH);
// RULE_OP(OP_UNARY_PREFIX, 1, TokenType_STAR);
// RULE_OP(OP_UNARY_POSTFIX, 1, TokenType_COMMA);

// FLOAT: PERIOD NUMBER
RULE_IMPL(FLOAT)
  SCAN(PERIOD);
  SCAN_ADD(NUMBER);
END_RULE

// RANGE: COMMA <EXPR>
RULE_IMPL(RANGE)
  SCAN(DBL_PERIOD);
  if (NEXT_TOKEN == TokenType_LPAR) {
    SCAN(LPAR);
    ROOT_APPEND(RULE(EXPR_OR));
    SCAN(RPAR);
  } else ROOT_APPEND(RULE(EXPR));
  SCAN(TPL_PERIOD);
  ROOT_APPEND(RULE(EXPR));
END_RULE

RULE_IMPL(FUNCTION_TYPE)
  ROOT_APPEND(RULE(TUPLE));
  SCAN(BIGARROW);
  ROOT_APPEND(RULE(TUPLE));
END_RULE

// LITERAL: <FLOAT> | NUMBER PERIOD <FLOAT> | HEX | OCTAL | BINARY
// FIRST(<FLOAT>) = PERIOD
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
  else if (NEXT_TOKEN == TokenType_QUOTE) SCAN_ADD(QUOTE);
  else if (NEXT_TOKEN == TokenType_STRING) SCAN_ADD(STRING);
  else SCAN_ADD(BINARY);
END_RULE

// LOAD: LOAD ID <LOAD_WHILE>
// LOAD_WHILE: PERIOD STAR | PERIOD ID | NULL
// FIRST(<LOAD_WHILE>) = PERIOD
RULE_IMPL(LOAD)
  SCAN(LOAD);
  SCAN_ADD(ID);
  // <LOAD_WHILE>
  while (NEXT_TOKEN == TokenType_PERIOD) {
    SCAN(PERIOD);
    if (NEXT_TOKEN == TokenType_STAR) { SCAN_ADD(STAR); break; }
    SCAN_ADD(ID);
  }
END_RULE

// METHOD_TYPE: <TUPLE> SMALLARROW <FUNCTION_TYPE>
RULE_IMPL(METHOD_TYPE)
  ROOT_APPEND(RULE(TUPLE));
  SCAN(SMALLARROW);
  ROOT_APPEND(RULE(FUNCTION_TYPE));
END_RULE

// PROGRAM: STMT_LIST
RULE_IMPL(PROGRAM)
  ROOT_APPEND(RULE(STMT_LIST));
  // SCAN(EOF)
END_RULE

// PRIMARY: <TUPLE> | <VAR> | <PRIMARY_2> | <LITERAL>
// PRIMARY_2: <PRIMARY> <CALL> | <PRIMARY> <METHOD>
// FIRST(<TUPLE>) = LPAR
// FIRST(<VAR>) = ID
// FIRST(<CALL>) = LPAR
// FIRST(<METHOD>) = SMALLARROW
// FIRST(<LITERAL>) = NUMBER | HEX | OCTAL | BINARY | PERIOD
RULE_IMPL(PRIMARY)
  AST* prefix = NULL, *call;
  if (NEXT_TOKEN == TokenType_LPAR) prefix = RULE(TUPLE);
  else if (NEXT_TOKEN == TokenType_ID) prefix = RULE(VAR);
  if (NEXT_TOKEN == TokenType_LPAR) {
    call = BRANCH_CREATE(CALL);
    BRANCH_APPEND(call, prefix);
    BRANCH_APPEND(call, RULE(TUPLE));
    return call;
  } else if (NEXT_TOKEN == TokenType_SMALLARROW){
    call = BRANCH_CREATE(METHOD);
    SCAN(SMALLARROW);
    BRANCH_APPEND(call, prefix);
    BRANCH_APPEND(call, SCAN(ID));
    BRANCH_APPEND(call, RULE(TUPLE));
    return call;
  }
  if (FIRST(7, TokenType_NUMBER, TokenType_HEX, TokenType_OCTAL, TokenType_BINARY,
            TokenType_PERIOD, TokenType_QUOTE, TokenType_STRING)) return RULE(LITERAL);
  return prefix;
END_RULE

RULE_IMPL(SIZEOF)
  SCAN(SIZEOF);
  RULE(EXPR);
END_RULE

RULE_IMPL(STMT)
  if(FIRST(4, TokenType_ID, TokenType_NUMBER, TokenType_STAR, TokenType_LOAD)) {
    ROOT_APPEND(RULE(EXPR));
    SCAN(SEMI);
  } else if (NEXT_TOKEN == TokenType_ENV) ROOT_APPEND(RULE(ENV));
  // TODO: handle CASE
  else ROOT_APPEND(RULE(BLOCK_STMT));
END_RULE

RULE_IMPL(STMT_LIST)
  if (FIRST(8,
        TokenType_ID, TokenType_NUMBER, TokenType_STAR, TokenType_IF,
        TokenType_SWITCH, TokenType_WHILE, TokenType_DO, TokenType_LOAD/*, TokenType_CASE */)) {
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

// TUPLE: LPAR <TUPLE_BODY> RPAR
// FOLLOW(<TUPLE_BODY>) = RPAR
RULE_IMPL(TUPLE)
  if (NEXT_TOKEN == TokenType_LPAR)
  {
    SCAN(LPAR);
    if (NEXT_TOKEN == TokenType_RPAR) SCAN(RPAR);
    else {
      ROOT_APPEND(RULE(TUPLE_BODY));
      SCAN(RPAR);
    }
  }
  else if (NEXT_TOKEN == TokenType_ID) return RULE(VAR);
  else return RULE(LITERAL);
END_RULE

// TUPLE_BODY: NULL | <EXPR> | <EXPR> COMMA <TUPLE_BODY>
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

// TYPE: ID | NUMBER | <TUPLE_TYPE> | TILDE <TYPE> | <TYPE> LBRACKET <EXPR> RBRACKET
// | <TUPLE_TYPE> BIG_ARROW <TUPLE_TYPE>
// | <TUPLE_TYPE> SMALL_ARROW <TUPLE_TYPE> BIG_ARROW <TUPLE_TYPE>
// FIRST(<TUPLE_TYPE>) = LPAR
// FOLLOW(<TUPLE_TYPE>) = SMALL_ARROW | BIG_ARROW
// FOLLOW(LBRACKET) = { FIRST(<EXPR>) - NULL }
RULE_IMPL(TYPE)
  AST *m, *s;

  // Handle simple types

  switch(NEXT_TOKEN) {
  case TokenType_ID:
    SCAN_ADD(ID);
    break;
  case TokenType_NUMBER:
    SCAN_ADD(NUMBER);
    break;
  case TokenType_LPAR:
    ROOT_APPEND(RULE(TUPLE_TYPE));
    break;
  case TokenType_BW_AND:
    SCAN_ADD(BW_AND);
    ROOT_APPEND(RULE(TYPE));
    break;
  default:
    break;
  }
  // if(FIRST(1, TokenType_ID)) SCAN_ADD(ID);
  // else if(FIRST(1, TokenType_NUMBER)) SCAN_ADD(NUMBER);
  // else if(FIRST(1, TokenType_LPAR)) {
  //   ROOT_APPEND(RULE(TUPLE_TYPE));
  // }
  // else if (FIRST(1, TokenType_STAR)) {
  //   s = SCAN(STAR);
  //   BRANCH_APPEND(m, RULE(TYPE));
  // }
  // else if (FIRST(1, TokenType_MUT)) {
  //   m = SCAN(MUT);
  //   BRANCH_APPEND(m, RULE(TYPE));
  // }

  // Handle methods and functions
  if (FIRST(2, TokenType_BIGARROW, TokenType_SMALLARROW)) {
    if (NEXT_TOKEN == TokenType_SMALLARROW) {
      SCAN(SMALLARROW);
      ROOT_APPEND(RULE(TUPLE_TYPE));
    }
    SCAN(BIGARROW);
    ROOT_APPEND(RULE(TUPLE_TYPE));
  } else while (NEXT_TOKEN == TokenType_LBRACKET) {
    SCAN(LBRACKET);
    ROOT_APPEND(RULE(EXPR));
    SCAN(RBRACKET);
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
// VAR: ID (COLON <TYPE> | PERIOD <VAR>) | STAR <VAR>
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

// WHILE: WHILE <EXPR> <BLOCK>
RULE_IMPL(WHILE)
  SCAN(WHILE);
  ROOT_APPEND(RULE(EXPR));
  ROOT_APPEND(RULE(BLOCK));
END_RULE

#undef X
