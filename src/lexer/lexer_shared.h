#define TokenTypeTableSimple \
  X(SEMI,         ';', "SEMI") \
  X(LBRACE,       '{', "LBRACE")\
  X(RBRACE,       '}', "RBRACE")\
  X(LPAR,         '(', "LPAR")\
  X(RPAR,         ')', "RPAR")\
  X(PERIOD,       '.', "PERIOD")\
  X(STAR,         '*', "STAR")\
  X(PLUS,         '+', "PLUS")\
  X(CARAT,        '^', "CARAT")\
  X(AND,          '&', "AND")\
  X(COMMA,        ',', "COMMA")\
  X(TYPE_ASSIGN,  ':', "COLON")\

#define TokenTypeTableOverload \
  X(MINUS,        '-', "MINUS")\
  X(SMALLARROW,   '-', "S-ARROW")\
  X(ASSIGN,       '=', "ASSIGN")\
  X(BIGARROW,     '=', "B-ARROW")\
  X(EQUALS,       '=', "EQUALS")\
  X(HEX,          '0', "HEX")\
  X(OCTAL,        '0', "OCTAL")\
  X(BINARY,       '0', "BINARY")\

#define TokenTypeTableStmts \
  X(NUMBER,      '\0', "NUMBER")\
  X(FLOAT,       '\0', "FLOAT")\
  X(ID,          '\0', "ID")\

#define TokenTypeTable \
  TokenTypeTableSimple\
  TokenTypeTableOverload\
  TokenTypeTableStmts

typedef enum {
#define X(name, first, str) TokenType_##name,
  TokenTypeTable
#undef X
  TokenType_COUNT
} TokenType;
