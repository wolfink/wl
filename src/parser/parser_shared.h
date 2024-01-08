#ifndef PARSER_SHARED_H
#define PARSER_SHARED_H

#define ListRules \
  X(STMT_LIST, "STMT_LIST") \
  X(TUPLE_BODY, "TUPLE_BODY") \
  X(TUPLE_TYPE_BODY, "TUPLE_TYPE_BODY")

#define StdRules \
  X(ASSIGN, "ASSIGN") \
  X(BLOCK, "BLOCK") \
  X(BRANCH, "BRANCH") \
  X(CALL, "CALL") \
  X(CASE, "CASE") \
  X(COLON_ASSIGN, "COLON_ASSIGN") \
  X(DATA_TYPE, "DATA_TYPE") \
  X(DO, "DO") \
  X(ELSE, "ELSE") \
  X(ENV, "ENV") \
  X(EXPR, "EXPR") \
  X(FACTOR, "FACTOR") \
  X(FLOAT, "FLOAT") \
  X(FUNCTION_TYPE, "FUNCTION_TYPE") \
  X(LITERAL, "LITERAL") \
  X(METHOD_TYPE, "METHOD_TYPE") \
  X(MUT_TYPE, "MUT_TYPE") \
  X(POINTER_TYPE, "POINTER_TYPE") \
  X(PROGRAM, "PROGRAM") \
  X(STMT, "STMT") \
  X(SWITCH, "SWITCH") \
  X(TAG, "TAG") \
  X(TERM, "TERM") \
  X(TUPLE, "TUPLE") \
  X(TUPLE_TYPE, "TUPLE_TYPE") \
  X(TUPLE_TYPE_BODY_TYPE, "TTB_TYPE") \
  X(TYPE, "TYPE") \
  X(WHILE, "WHILE") \
  X(VAR, "VAR")

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
} AST_Type;

#endif
