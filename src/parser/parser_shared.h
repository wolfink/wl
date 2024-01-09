#ifndef PARSER_SHARED_H
#define PARSER_SHARED_H

#define ListRules \
  X(STMT_LIST, "STMT_LIST") \
  X(TUPLE_BODY, "TUPLE_BODY") \
  X(TUPLE_TYPE_BODY, "TUPLE_TYPE_BODY")

// Term1 : +, -
// Term2 : <, <=, >, >=
// Term3 : ==, !=
// Term4 : &
// Term5 : ^
// Term6 : |
// Term7 : &&
// Term8 : ||
// Term9 : =, :=, +=, -=, *=, /=, etc.
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
  X(TERM0, "TERM0") \
  X(TERM1, "TERM1") \
  X(TERM2, "TERM2") \
  X(TERM3, "TERM3") \
  X(TERM4, "TERM4") \
  X(TERM5, "TERM5") \
  X(TERM6, "TERM6") \
  X(TERM7, "TERM7") \
  X(TERM8, "TERM8") \
  X(TERM9, "TERM9") \
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
