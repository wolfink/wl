#ifndef PARSER_INTERNAL_H
#define PARSER_INTERNAL_H

#include <lexer.h>
#include <parser.h>

AST* scan(Arena*, Parser*, TokenType);
const Token* parser_next_token(Parser* p);

#endif
