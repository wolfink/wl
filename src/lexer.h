#include "util.h"

#ifndef LEXER_INTERNAL_H
typedef void Lexer;
#endif

extern Lexer*  lexer_create(Arena*, string* in);
extern string* lexer_to_string(Arena*, Lexer*);
extern string* lexer_get_value_at_index(Arena*, Lexer*, size_t index);
extern size_t  lexer_get_len(Lexer*);
