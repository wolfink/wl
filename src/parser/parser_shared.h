#ifndef PARSER_SHARED_H
#define PARSER_SHARED_H

#define ListRules \
  X(STMT_LIST, "STMT_LIST") \
  X(TUPLE_BODY, "TUPLE_BODY") \
  X(TUPLE_TYPE_BODY, "TUPLE_TYPE_BODY")

// EXPR_SUM       : +, -
// EXPR_REL       : <, <=, >, >=
// EXPR_EQ        : ==, !=
// EXPR_BW_AND    : &
// *TODO*         : ^
// EXPR_BW_OR     : |
// EXPR_AND       : &&
// EXPR_OR        : ||
// ASSIGN         : =, :=, +=, -=, *=, /=, etc.
#define StdRules \
  X(ASSIGN, "ASSIGN") \
  X(BLOCK, "BLOCK") \
  X(BRANCH, "BRANCH") \
  X(BLOCK_STMT, "BLOCK_STMT")\
  X(CALL, "CALL") \
  X(CASE, "CASE") \
  X(COLON_ASSIGN, "COLON_ASSIGN") \
  X(DATA_TYPE, "DATA_TYPE") \
  X(DO, "DO") \
  X(ELSE, "ELSE") \
  X(ENV, "ENV") \
  X(EXPR, "EXPR") \
  X(EXPR_OR, "EXPR_OR") \
  X(EXPR_AND, "EXPR_AND") \
  X(EXPR_BW_OR, "EXPR_BW_OR") \
  X(EXPR_BW_AND, "EXPR_BW_AND") \
  X(EXPR_EQ, "EXPR_EQ") \
  X(EXPR_REL, "EXPR_REL") \
  X(EXPR_SUM, "EXPR_SUM") \
  X(EXPR_MUL, "EXPR_MUL") \
  X(EXPR_UNARY, "EXPR_UNARY") \
  X(FACTOR, "FACTOR") \
  X(FLOAT, "FLOAT") \
  X(FUNCTION_TYPE, "FUNCTION_TYPE") \
  X(LITERAL, "LITERAL") \
  X(METHOD, "METHOD") \
  X(METHOD_TYPE, "METHOD_TYPE") \
  X(MUT_TYPE, "MUT_TYPE") \
  X(MUL_EXPR, "MUL_EXPR") \
  X(OP_EQ, "OP_EQ") \
  X(OP_REL, "OP_REL") \
  X(OP_SUM, "OP_SUM") \
  X(OP_MUL, "OP_MUL") \
  X(OP_UNARY, "OP_UNARY") \
  X(POINTER_TYPE, "POINTER_TYPE") \
  X(PROGRAM, "PROGRAM") \
  X(PRIMARY, "PRIMARY") \
  X(STMT, "STMT") \
  X(SWITCH, "SWITCH") \
  X(SUM_EXPR, "SUM_EXPR") \
  X(TAG, "TAG") \
  X(TERM, "TERM") \
  X(TUPLE, "TUPLE") \
  X(TUPLE_TYPE, "TUPLE_TYPE") \
  X(TUPLE_TYPE_BODY_TYPE, "TTB_TYPE") \
  X(TYPE, "TYPE") \
  X(UNARY, "UNARY") \
  X(VAR, "VAR")\
  X(WHILE, "WHILE") \

#define Rules \
  StdRules \
  ListRules

typedef enum
{
#define X(name, str) \
  ASTType_##name,
  Rules
#undef X
  ASTType_ID,
  ASTType_NUMBER,
  ASTType_TOKEN,
  ASTType_COUNT,
} ASTType;

#endif
