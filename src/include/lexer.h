#ifndef LEXER_H
#define LEXER_H

#include "util.h"

#ifndef LEXER_INTERNAL_H

#include "../lexer/lexer_shared.h"
typedef void Lexer;

#endif

extern string*   token_type_tostr(Arena*, TokenType);
extern Lexer*    lexer_create(Arena*, string* in);
extern string*   lexer_to_string(Arena*, Lexer*);
extern string*   lexer_get_value_at_index(Arena*, Lexer*, size_t index);
extern TokenType lexer_get_token_type_at_index(Lexer*, size_t index);
extern size_t    lexer_get_len(Lexer*);

#endif
