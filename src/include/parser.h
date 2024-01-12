#ifndef PARSER_H
#define PARSER_H

#include "util.h"
#include "lexer.h"

#ifndef PARSER_INTERNAL_H

#include "../parser/parser_shared.h"
typedef void Parser;

#endif

Parser* parser_create(Arena*, Lexer*);
string*  parser_to_string(Arena*, Parser*);

#endif
