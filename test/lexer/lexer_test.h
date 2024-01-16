#ifndef LEXER_TEST_H
#define LEXER_TEST_H

#include "../test.h"
#include "../../src/include/util.h"
#include "../../src/include/lexer.h"


//TODO: add X(COLON_ARROW, ":>")
#define TokenTable\
  X(ASSIGN, "=")\
  X(BW_AND, "&")\
  X(BW_OR, "|")\
  X(CARAT, "^")\
  X(COLON, ":")\
  X(COMMA, ",")\
  X(FSLASH, "/")\
  X(LANGLE, "<")\
  X(LBRACE, "{")\
  X(LPAR, "(")\
  X(MINUS, "-")\
  X(PERIOD, ".")\
  X(PLUS, "+")\
  X(RANGLE, ">")\
  X(RBRACE, "}")\
  X(RPAR, ")")\
  X(SEMI, ";")\
  X(STAR, "*")\
  X(SMALLARROW, "->")\
  X(BIGARROW, "=>")\
  X(EQUALS, "==")\
  X(AND, "&&")\
  X(OR, "||")\
  X(COLON_ASSIGN, ":=")\
  X(DBL_COLON, "::")\
  X(COLON_LARROW, "<:")\
  X(COLON_RARROW, ":>")\
  X(IF, "if")\
  X(DO, "do")\
  X(ENV, "env")\
  X(FOR, "for")\
  X(MUT, "mut")\
  X(ELSE, "else")\
  X(BREAK, "break")\
  X(WHILE, "while")\
  X(INLINE, "inline")\
  X(SWITCH, "switch")\
  X(OCTAL, "01572")\
  X(BINARY, "0b0110")\
  X(HEX, "0x2afd")\
  X(NUMBER, "1200")\
  X(ID, "test")

typedef struct LexerOutput {
  TokenType t;
  char val[32];
} LexerOutput;

static LexerOutput test_commands_expected[NUM_COMMANDS][100] = {
  {
    { TokenType_ID, "x" },
    { TokenType_COLON, "" },
    { TokenType_ID, "int" },
    { TokenType_ASSIGN, "" },
    { TokenType_NUMBER, "1000"},
    { TokenType_SEMI, "" }
  }, {
    { TokenType_ID, "y" },
    { TokenType_COLON, "" },
    { TokenType_ID, "float" },
    { TokenType_ASSIGN, "" },
    { TokenType_NUMBER, "1" },
    { TokenType_PERIOD, "" },
    { TokenType_NUMBER, "200" },
    { TokenType_SEMI, "" }
  }, {
    { TokenType_ID, "z" },
    { TokenType_COLON, "" },
    { TokenType_LPAR, "" },
    { TokenType_ID, "x" },
    { TokenType_COLON, "" },
    { TokenType_ID, "int" },
    { TokenType_COMMA, "" },
    { TokenType_ID, "y" },
    { TokenType_COLON, "" },
    { TokenType_ID, "float" },
    { TokenType_RPAR, ""},
    { TokenType_BIGARROW, ""},
    { TokenType_LPAR, "" },
    { TokenType_ID, "ret" },
    { TokenType_COLON, "" },
    { TokenType_ID, "int" },
    { TokenType_RPAR, "" },
    { TokenType_SEMI, "" }
  }, {
    { TokenType_IF, "" },
    { TokenType_ID, "x" },
    { TokenType_RANGLE, "" },
    { TokenType_NUMBER, "3" },
    { TokenType_AND, "" },
    { TokenType_ID, "y" },
    { TokenType_LANGLE, "" },
    { TokenType_NUMBER, "2" },
    { TokenType_LBRACE, "" },
    { TokenType_ID, "x" },
    { TokenType_ASSIGN, "" },
    { TokenType_ID, "z" },
    { TokenType_LPAR, "" },
    { TokenType_ID, "x" },
    { TokenType_COMMA, "" },
    { TokenType_ID, "y" },
    { TokenType_RPAR, "" },
    { TokenType_SEMI, "" },
    { TokenType_RBRACE, "" },
    { TokenType_ELSE, "" },
    { TokenType_LBRACE, "" },
    { TokenType_ID, "x" },
    { TokenType_ASSIGN, "" },
    { TokenType_NUMBER, "1" },
    { TokenType_SEMI, "" },
    { TokenType_RBRACE, "" }
  }, {
    { TokenType_ID, "x" },
    { TokenType_ASSIGN, "" },
    { TokenType_ID, "x" },
    { TokenType_STAR, "" },
    { TokenType_NUMBER, "2" },
    { TokenType_PLUS, "" },
    { TokenType_NUMBER, "1000" },
    { TokenType_MINUS, "" },
    { TokenType_NUMBER, "30" },
    { TokenType_SEMI, "" }
  }, {
    { TokenType_ID, "w" },
    { TokenType_COLON_ASSIGN, "" },
    { TokenType_HEX, "1adf" },
    { TokenType_SEMI, "" }
  }, {
    { TokenType_ID, "a" },
    { TokenType_COLON_ASSIGN, "" },
    { TokenType_BINARY, "0011" },
    { TokenType_SEMI, "" }
  }, {
    { TokenType_ID, "b" },
    { TokenType_COLON_ASSIGN, "" },
    { TokenType_OCTAL, "200" },
    { TokenType_SEMI, "" }
  }, {
    { TokenType_ID, "c" },
    { TokenType_COLON_ASSIGN, "" },
    { TokenType_PERIOD, "" },
    { TokenType_NUMBER, "300" },
    { TokenType_SEMI, "" }
  }, {
    { TokenType_ID, "d" },
    { TokenType_COLON_ASSIGN, "" },
    { TokenType_PERIOD, "" },
    { TokenType_NUMBER, "400" },
    { TokenType_SEMI, "" }
  }, {
    { TokenType_ID, "e" },
    { TokenType_COLON, "" },
    { TokenType_LPAR, "" },
    { TokenType_ID, "a" },
    { TokenType_COLON, "" },
    { TokenType_ID, "int" },
    { TokenType_COMMA, "" },
    { TokenType_ID, "b" },
    { TokenType_COLON, "" },
    { TokenType_ID, "int" },
    { TokenType_RPAR, "" },
    { TokenType_ASSIGN, "" },
    { TokenType_LPAR, "" },
    { TokenType_NUMBER, "1" },
    { TokenType_COMMA, "" },
    { TokenType_NUMBER, "2" },
    { TokenType_RPAR, "" },
    { TokenType_SEMI, "" }
  }, {
    { TokenType_ID, "f" },
    { TokenType_ASSIGN, "" },
    { TokenType_ID, "e" },
    { TokenType_PERIOD, "" },
    { TokenType_ID, "a" },
    { TokenType_SEMI, "" }
  }
};


#endif
