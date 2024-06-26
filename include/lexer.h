#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>
#include <util/arena.h>
#include <util/string.h>
#include <util/vector.h>

#define TokenTypeTable1 \
  X(ASSIGN,       '=',  "\"=\"")\
  X(BW_AND,       '&',  "\"&\"")\
  X(BW_OR,        '|',  "\"|\"")\
  X(CARAT,        '^',  "\"^\"")\
  X(COLON,        ':',  "\":\"")\
  X(COMMA,        ',',  "\",\"")\
  X(FSLASH,       '/',  "\"/\"") \
  X(LANGLE,       '<',  "\"<\"")\
  X(LBRACE,       '{',  "\"{\"")\
  X(LBRACKET,     '[',  "\"[\"")\
  X(LPAR,         '(',  "\"(\"")\
  X(MINUS,        '-',  "\"-\"")\
  X(NOT,          '!',  "\"!\"")\
  X(PERIOD,       '.',  "\".\"")\
  X(PERCENT,      '%',  "\"%\"")\
  X(PLUS,         '+',  "\"+\"")\
  X(RANGLE,       '>',  "\">\"")\
  X(RBRACE,       '}',  "\"}\"")\
  X(RBRACKET,     ']',  "\"]\"")\
  X(RPAR,         ')',  "\")\"")\
  X(SEMI,         ';',  "\";\"") \
  X(STAR,         '*',  "\"*\"")\
  X(TILDE,        '~',  "\"~\"")\
  X(EOF,         '\0',  "EOF")\

#define TokenTypeTable2 \
  X(SMALLARROW,    CHAR_SUM_2('-', '>'), "\"->\"")\
  X(BIGARROW,      CHAR_SUM_2('=', '>'), "\"=>\"")\
  X(EQUALS,        CHAR_SUM_2('=', '='), "\"==\"")\
  X(AND_EQUALS,    CHAR_SUM_2('&', '='), "\"&=\"")\
  X(CARAT_EQUALS,  CHAR_SUM_2('^', '='), "\"^=\"")\
  X(GT_EQUALS,     CHAR_SUM_2('>', '='), "\">=\"")\
  X(MINUS_EQUALS,  CHAR_SUM_2('-', '='), "\"-=\"")\
  X(LT_EQUALS,     CHAR_SUM_2('<', '='), "\"<=\"")\
  X(MOD_EQUALS,    CHAR_SUM_2('%', '='), "\"%=\"")\
  X(OR_EQUALS,     CHAR_SUM_2('|', '='), "\"|=\"")\
  X(PLUS_EQUALS,   CHAR_SUM_2('+', '='), "\"+=\"")\
  X(SLASH_EQUALS,  CHAR_SUM_2('/', '='), "\"/=\"")\
  X(STAR_EQUALS,   CHAR_SUM_2('*', '='), "\"*=\"")\
  X(NOT_EQUALS,    CHAR_SUM_2('!', '='), "\"!=\"")\
  X(AND,           CHAR_SUM_2('&', '&'), "\"&&\"")\
  X(OR,            CHAR_SUM_2('|', '|'), "\"||\"")\
  X(DBL_PERIOD,    CHAR_SUM_2('.', '.'), "\"..\"")\
  X(COLON_ASSIGN,  CHAR_SUM_2(':', '='), "\":=\"")\
  X(DBL_COLON,     CHAR_SUM_2(':', ':'), "\"::\"")\
  X(COLON_LARROW,  CHAR_SUM_2('<', ':'), "\"<:\"")\
  X(COLON_RARROW,  CHAR_SUM_2(':', '>'), "\":>\"")\
  X(SHIFT_LEFT,    CHAR_SUM_2('<', '<'), "\"<<\"")\
  X(SHIFT_RIGHT,   CHAR_SUM_2('>', '>'), "\">>\"")\
  X(IF,            CHAR_SUM_2('i', 'f'), "if")\
  X(DO,            CHAR_SUM_2('d', 'o'), "do")\

#define TokenTypeTable3 \
  X(ENV,          CHAR_SUM_3('e', 'n', 'v'), "env")\
  X(FOR,          CHAR_SUM_3('f', 'o', 'r'), "for")\
  X(MUT,          CHAR_SUM_3('m', 'u', 't'), "mut")\
  X(SL_EQUALS,    CHAR_SUM_3('<', '<', '='), "\"<<=\"")\
  X(SR_EQUALS,    CHAR_SUM_3('>', '>', '='), "\">>=\"")\
  X(TPL_PERIOD,   CHAR_SUM_3('.', '.', '.'), "\"...\"")

#define TokenTypeTable4 \
  X(ELSE,         CHAR_SUM_4('e', 'l', 's', 'e'), "else")\
  X(ENUM,         CHAR_SUM_4('e', 'n', 'u', 'm'), "enum")\
  X(LOAD,         CHAR_SUM_4('l', 'o', 'a', 'd'), "load")\

#define TokenTypeTable5 \
  X(BREAK,        CHAR_SUM_5('b', 'r', 'e', 'a', 'k'), "break")\
  X(WHILE,        CHAR_SUM_5('w', 'h', 'i', 'l', 'e'), "while")\

#define TokenTypeTable6 \
  X(INLINE,       CHAR_SUM_6('i', 'n', 'l', 'i', 'n', 'e'), "inline")\
  X(SWITCH,       CHAR_SUM_6('s', 'w', 'i', 't', 'c', 'h'), "switch")\
  X(SIZEOF,       CHAR_SUM_6('s', 'i', 'z', 'e', 'o', 'f'), "sizeof")\

#define TokenTypeTableUnique \
  X(NEWLINE, '\n', "\"\\n\"")\
  X(QUOTE, '\'', "char literal")\
  X(DBL_QUOTE,    '\"', "string literal")\
  X(MACRO, '#', "macro")\
  X(LINE_COMMENT, CHAR_SUM_2('/', '/'), "line comment")\
  X(COMMENT_START, CHAR_SUM_2('/', '*'), "comment start")\


#define TokenTypeTableNumber \
  X(OCTAL,        0, "octal")\
  X(BINARY,       0, "binary")\
  X(HEX,          0, "hexadecimal")\

#define TokenTypeTableStmts \
  X(NUMBER,      0, "number")\
  X(ID,          0, "identifier")\
  X(STRING,      0, "string")\

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

typedef struct {
  TokenType type;
  const string* value;
  size_t start;
  size_t end;
} Token;

#ifdef LEXER_INTERNAL

vector_template(Token);
typedef vector_Token Line;
typedef vector_object vector_Line;

typedef struct {
  Arena* mem;
  size_t curr_line;
  const char* filename;
  vector_Line* lines;
} Lexer;

# else

typedef struct {} Lexer;

#endif

extern string*   token_type_tostr(Arena*, TokenType);

Token*  token_create(Arena* arena, TokenType t, const string* value, size_t start, size_t end);
string* token_to_string(Arena*, Token*);

extern Lexer*      lexer_create(Arena*, const char* filename);
extern int         lexer_scan(Lexer* lex, string* in);
extern const char* lexer_get_filename(Lexer*);
extern string*     lexer_to_string(Arena*, Lexer*);
// extern string*   lexer_get_value_at_index(Arena*, Lexer*, size_t index);
// extern TokenType lexer_get_token_type_at_index(Lexer*, size_t index);
const Token*       lexer_get_token(Lexer* lex, size_t line_no, size_t index);
extern size_t      lexer_get_num_lines(Lexer* lex);
extern size_t      lexer_get_line_len(Lexer* lex, size_t line_no);

#endif
