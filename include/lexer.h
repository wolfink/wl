#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>

#ifndef LEXER_INTERNAL_H

#include "../src/lexer/lexer_shared.h"
typedef void Lexer;

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
