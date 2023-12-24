#define CHAR_SUM_2(a, b)                   (T(b) << 8)  + T(a)
#define CHAR_SUM_3(a, b, c)                (T(c) << 16) + (T(b) << 8)  + T(a)
#define CHAR_SUM_4(a, b, c, d)             (T(d) << 24) + (T(c) << 16) + (T(b) << 8)  + T(a)
#define CHAR_SUM_5(a, b, c, d, e)          (T(e) << 32) + (T(d) << 24) + (T(c) << 16) + (T(b) << 8)  + T(a)
#define CHAR_SUM_6(a, b, c, d, e, f)       (T(f) << 40) + (T(e) << 32) + (T(d) << 24) + (T(c) << 16) + (T(b) << 8)  + T(a)
#define CHAR_SUM_7(a, b, c, d, e, f, g)    (T(g) << 48) + (T(f) << 40) + (T(e) << 32) + (T(d) << 24) + (T(c) << 16) + (T(b) << 8)  + T(a)
#define CHAR_SUM_8(a, b, c, d, e, f, g, h) (T(h) << 56) + (T(g) << 48) + (T(f) << 40) + (T(e) << 32) + (T(d) << 24) + (T(c) << 16) + (T(b) << 8) + a

#define TokenTypeTable1 \
  X(ASSIGN,       '=', "ASSIGN")\
  X(BW_AND,       '&', "BW-AND")\
  X(BW_OR,        '|', "BW-OR")\
  X(CARAT,        '^', "CARAT")\
  X(COLON,        ':', "COLON")\
  X(COMMA,        ',', "COMMA")\
  X(LANGLE,       '<', "LANGLE")\
  X(LBRACE,       '{', "LBRACE")\
  X(LPAR,         '(', "LPAR")\
  X(MINUS,        '-', "MINUS")\
  X(PERIOD,       '.', "PERIOD")\
  X(PLUS,         '+', "PLUS")\
  X(RANGLE,       '>', "RANGLE")\
  X(RBRACE,       '}', "RBRACE")\
  X(RPAR,         ')', "RPAR")\
  X(SEMI,         ';', "SEMI") \
  X(STAR,         '*', "STAR")\

#define TokenTypeTable2 \
  X(SMALLARROW,    CHAR_SUM_2('-', '>'), "S-ARROW")\
  X(BIGARROW,      CHAR_SUM_2('=', '>'), "B-ARROW")\
  X(EQUALS,        CHAR_SUM_2('=', '='), "EQUALS")\
  X(AND,           CHAR_SUM_2('&', '&'), "AND")\
  X(OR,            CHAR_SUM_2('|', '|'), "OR")\
  X(COLON_ASSIGN,  CHAR_SUM_2(':', '='), "COLON-ASSIGN")\
  X(DBL_COLON,     CHAR_SUM_2(':', ':'), "DBL-COLON")\
  X(COLON_ARROW,   CHAR_SUM_2('<', ':'), "COLON-ARROW")\
  X(IF,            CHAR_SUM_2('i', 'f'), "IF")\
  X(DO,            CHAR_SUM_2('d', 'o'), "DO")\

#define TokenTypeTable3 \
  X(ENV,          CHAR_SUM_3('e', 'n', 'v'), "ENV")\
  X(FOR,          CHAR_SUM_3('f', 'o', 'r'), "FOR")\
  X(MUT,          CHAR_SUM_3('m', 'u', 't'), "MUT")\

#define TokenTypeTable4 \
  X(ELSE,         CHAR_SUM_4('e', 'l', 's', 'e'), "ELSE")\

#define TokenTypeTable5 \
  X(BREAK,        CHAR_SUM_5('b', 'r', 'e', 'a', 'k'), "BREAK")\
  X(WHILE,        CHAR_SUM_5('w', 'h', 'i', 'l', 'e'), "WHILE")\

#define TokenTypeTable6 \
  X(INLINE,       CHAR_SUM_6('i', 'n', 'l', 'i', 'n', 'e'), "INLINE")\
  X(SWITCH,       CHAR_SUM_6('s', 'w', 'i', 't', 'c', 'h'), "SWITCH")\

#define TokenTypeTableNumber \
  X(OCTAL,        0, "OCTAL")\
  X(BINARY,       0, "BINARY")\
  X(HEX,          0, "HEX")\

#define TokenTypeTableStmts \
  X(NUMBER,      0, "NUMBER")\
  X(ID,          0, "ID")\

#define TokenTypeTable \
  TokenTypeTable1 \
  TokenTypeTable2 \
  TokenTypeTable3 \
  TokenTypeTable4 \
  TokenTypeTable5 \
  TokenTypeTable6 \
  TokenTypeTableNumber \
  TokenTypeTableStmts \

typedef enum {
#define X(name, first, str) TokenType_##name,
  TokenTypeTable
#undef X
  TokenType_COUNT
} TokenType;
