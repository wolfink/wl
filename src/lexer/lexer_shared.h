#define CHAR_SUM_2(a, b)                   (T(b) << 8)  + T(a)
#define CHAR_SUM_3(a, b, c)                (T(c) << 16) + (T(b) << 8)  + T(a)
#define CHAR_SUM_4(a, b, c, d)             (T(d) << 24) + (T(c) << 16) + (T(b) << 8)  + T(a)
#define CHAR_SUM_5(a, b, c, d, e)          (T(e) << 32) + (T(d) << 24) + (T(c) << 16) + (T(b) << 8)  + T(a)
#define CHAR_SUM_6(a, b, c, d, e, f)       (T(f) << 40) + (T(e) << 32) + (T(d) << 24) + (T(c) << 16) + (T(b) << 8)  + T(a)
#define CHAR_SUM_7(a, b, c, d, e, f, g)    (T(g) << 48) + (T(f) << 40) + (T(e) << 32) + (T(d) << 24) + (T(c) << 16) + (T(b) << 8)  + T(a)
#define CHAR_SUM_8(a, b, c, d, e, f, g, h) (T(h) << 56) + (T(g) << 48) + (T(f) << 40) + (T(e) << 32) + (T(d) << 24) + (T(c) << 16) + (T(b) << 8) + a

#define TokenTypeTable1 \
  X(ASSIGN,       '=',  "ASSIGN")\
  X(BW_AND,       '&',  "BW-AND")\
  X(BW_OR,        '|',  "BW-OR")\
  X(CARAT,        '^',  "CARAT")\
  X(COLON,        ':',  "COLON")\
  X(COMMA,        ',',  "COMMA")\
  X(DBL_QUOTE,    '\"', "DBL-QUOTE")\
  X(FSLASH,       '/',  "FSLASH") \
  X(HASH,         '#',  "HASH")\
  X(LANGLE,       '<',  "LANGLE")\
  X(LBRACE,       '{',  "LBRACE")\
  X(LBRACKET,     '[',  "LBRACKET")\
  X(LPAR,         '(',  "LPAR")\
  X(MINUS,        '-',  "MINUS")\
  X(PERIOD,       '.',  "PERIOD")\
  X(PERCENT,      '%',  "PERCENT")\
  X(PLUS,         '+',  "PLUS")\
  X(QUOTE,        '\'', "QUOTE")\
  X(RANGLE,       '>',  "RANGLE")\
  X(RBRACE,       '}',  "RBRACE")\
  X(RBRACKET,     ']',  "RBRACKET")\
  X(RPAR,         ')',  "RPAR")\
  X(SEMI,         ';',  "SEMI") \
  X(STAR,         '*',  "STAR")\
  X(EOF,         '\0',  "EOF")\

#define TokenTypeTable2 \
  X(SMALLARROW,    CHAR_SUM_2('-', '>'), "S-ARROW")\
  X(BIGARROW,      CHAR_SUM_2('=', '>'), "B-ARROW")\
  X(EQUALS,        CHAR_SUM_2('=', '='), "EQUALS")\
  X(AND_EQUALS,    CHAR_SUM_2('&', '='), "AND-EQUALS")\
  X(CARAT_EQUALS,  CHAR_SUM_2('^', '='), "CARAT-EQUALS")\
  X(GT_EQUALS,     CHAR_SUM_2('>', '='), "GT-OR-EQUALS")\
  X(MINUS_EQUALS,  CHAR_SUM_2('-', '='), "MINUS-EQUALS")\
  X(LT_EQUALS,     CHAR_SUM_2('<', '='), "LT-OR-EQUALS")\
  X(MOD_EQUALS,    CHAR_SUM_2('%', '='), "MOD-EQUALS")\
  X(OR_EQUALS,     CHAR_SUM_2('|', '='), "OR-EQUALS")\
  X(PLUS_EQUALS,   CHAR_SUM_2('+', '='), "PLUS-EQUALS")\
  X(SLASH_EQUALS,  CHAR_SUM_2('/', '='), "SLASH-EQUALS")\
  X(STAR_EQUALS,   CHAR_SUM_2('*', '='), "STAR-EQUALS")\
  X(AND,           CHAR_SUM_2('&', '&'), "AND")\
  X(OR,            CHAR_SUM_2('|', '|'), "OR")\
  X(DBL_PERIOD,    CHAR_SUM_2('.', '.'), "DBL-PERIOD")\
  X(COLON_ASSIGN,  CHAR_SUM_2(':', '='), "COLON-ASSIGN")\
  X(DBL_COLON,     CHAR_SUM_2(':', ':'), "DBL-COLON")\
  X(COLON_LARROW,  CHAR_SUM_2('<', ':'), "COLON-LARROW")\
  X(COLON_RARROW,  CHAR_SUM_2(':', '>'), "COLON-RARROW")\
  X(SHIFT_LEFT,    CHAR_SUM_2('<', '<'), "SHIFT-LEFT")\
  X(SHIFT_RIGHT,   CHAR_SUM_2('>', '>'), "SHIFT-RIGHT")\
  X(IF,            CHAR_SUM_2('i', 'f'), "IF")\
  X(DO,            CHAR_SUM_2('d', 'o'), "DO")\

#define TokenTypeTable3 \
  X(ENV,          CHAR_SUM_3('e', 'n', 'v'), "ENV")\
  X(FOR,          CHAR_SUM_3('f', 'o', 'r'), "FOR")\
  X(MUT,          CHAR_SUM_3('m', 'u', 't'), "MUT")\
  X(SL_EQUALS,    CHAR_SUM_3('<', '<', '='), "SHIFT-LEFT-EQUALS")\
  X(SR_EQUALS,    CHAR_SUM_3('>', '>', '='), "SHIFT-RIGHT-EQUALS")\
  X(TPL_PERIOD,   CHAR_SUM_3('.', '.', '.'), "TPL-PERIOD")

#define TokenTypeTable4 \
  X(ELSE,         CHAR_SUM_4('e', 'l', 's', 'e'), "ELSE")\
  X(ENUM,         CHAR_SUM_4('e', 'n', 'u', 'm'), "ENUM")\
  X(LOAD,         CHAR_SUM_4('l', 'o', 'a', 'd'), "LOAD")\

#define TokenTypeTable5 \
  X(BREAK,        CHAR_SUM_5('b', 'r', 'e', 'a', 'k'), "BREAK")\
  X(WHILE,        CHAR_SUM_5('w', 'h', 'i', 'l', 'e'), "WHILE")\

#define TokenTypeTable6 \
  X(INLINE,       CHAR_SUM_6('i', 'n', 'l', 'i', 'n', 'e'), "INLINE")\
  X(SWITCH,       CHAR_SUM_6('s', 'w', 'i', 't', 'c', 'h'), "SWITCH")\

#define TokenTypeTableUnique \
  X(NEWLINE, '\n', "NEWLINE")\
  X(LINE_COMMENT, CHAR_SUM_2('/', '/'), "LINE-COMMENT")\
  X(COMMENT_START, CHAR_SUM_2('/', '*'), "COMMENT-START")\


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
  TokenTypeTableUnique \
  TokenTypeTableNumber \
  TokenTypeTableStmts \

typedef enum {
#define X(name, first, str) TokenType_##name,
  TokenTypeTable
#undef X
  TokenType_COUNT
} TokenType;
