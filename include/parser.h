#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>

#ifndef PARSER_INTERNAL_H

#include "../src/parser/parser_shared.h"
typedef void Parser;
typedef void AST;
typedef void Lexer;
typedef void Arena;
typedef void string;

#endif

Parser*  parser_create(Arena*, Lexer*);
AST*     parser_generate_ast(Parser*);

extern AST* ast_create(Arena*, ASTType, Lexer*);
extern void ast_append(Arena*, AST*, AST*);
extern string* ast_to_string(Arena*, AST*, int indent);
extern int  ast_is_token(AST*);
extern size_t ast_get_num_children(AST*);
extern AST*  ast_get_child(AST*, size_t);
extern void ast_set_child(AST*, size_t index, AST* value);
extern int  ast_get_type(AST*);

#endif
