#define TokenTypeTableSimple \
  X(CARAT,        '^', "CARAT")\
  X(COMMA,        ',', "COMMA")\
  X(LBRACE,       '{', "LBRACE")\
  X(LPAR,         '(', "LPAR")\
  X(PERIOD,       '.', "PERIOD")\
  X(PLUS,         '+', "PLUS")\
  X(RANGLE,       '>', "RANGLE")\
  X(RBRACE,       '}', "RBRACE")\
  X(RPAR,         ')', "RPAR")\
  X(SEMI,         ';', "SEMI") \
  X(STAR,         '*', "STAR")\

#define TokenTypeTableMinus \
  X(SMALLARROW,    '>', "S-ARROW")\

#define TokenTypeTableAssign \
  X(BIGARROW,      '>', "B-ARROW")\
  X(EQUALS,        '=', "EQUALS")\

#define TokenTypeTableAnd \
  X(AND,           '&', "AND")\

#define TokenTypeTableOr \
  X(OR,            '|', "OR")\

#define TokenTypeTableColon \
  X(COLON_ASSIGN,  '=', "COLON-ASSIGN")\
  X(DBL_COLON,     ':', "DBL-COLON")\

#define TokenTypeTableAngle \
  X(COLON_ARROW,   ':', "COLON-ARROW")\

#define TokenTypeTableZero \
  X(BINARY,       'b', "BINARY")\
  X(HEX,          'x', "HEX")\

#define TokenTypeTableOverload \
  X(ASSIGN,       '=', "ASSIGN")\
  X(BW_AND,       '&', "BW-AND")\
  X(BW_OR,        '|', "BW-OR")\
  X(COLON,        ':', "COLON")\
  X(LANGLE,       '<', "LANGLE")\
  X(MINUS,        '-', "MINUS")\
  X(OCTAL,        '0', "OCTAL")\
  TokenTypeTableAssign \
  TokenTypeTableAnd \
  TokenTypeTableAngle \
  TokenTypeTableColon \
  TokenTypeTableMinus \
  TokenTypeTableOr \
  TokenTypeTableZero \

#define TokenTypeTableKeyword \
  X(BREAK,        'b', "BREAK")\
  X(DO,           'd', "DO")\
  X(ELSE,         'e', "ELSE")\
  X(ENV,          'e', "ENV")\
  X(FOR,          'f', "FOR")\
  X(IF,           'i', "IF")\
  X(INLINE,       'i', "INLINE")\
  X(MUT,          'm', "MUT")\
  X(SWITCH,       's', "SWITCH")\
  X(WHILE,        'w', "WHILE")\

#define TokenTypeTableStmts \
  X(NUMBER,      '\0', "NUMBER")\
  X(FLOAT,       '\0', "FLOAT")\
  X(ID,          '\0', "ID")\

#define TokenTypeTable \
  TokenTypeTableSimple\
  TokenTypeTableOverload\
  TokenTypeTableKeyword\
  TokenTypeTableStmts

typedef enum {
#define X(name, first, str) TokenType_##name,
  TokenTypeTable
#undef X
  TokenType_COUNT
} TokenType;
