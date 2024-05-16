#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>
#include <lexer.h>
#include <ast.h>

#ifdef PARSER_INTERNAL
typedef struct
{
  Arena* context;
  size_t token_idx;
  size_t line_no;
  Lexer* lexer;
} Parser;
#else
typedef struct {} Parser;
#endif

Parser*  parser_create(Arena*, Lexer*);
AST*     parser_generate_ast(Parser*);
#ifdef PARSER_INTERNAL
AST* scan(Arena*, Parser*, TokenType);
const Token* parser_next_token(Parser* p);
#endif

extern AST* ast_create(Arena*, ASTType, Lexer*);
extern void ast_append(Arena*, AST*, AST*);
extern string* ast_to_string(Arena*, AST*, int indent);
extern int  ast_is_token(AST*);
extern size_t ast_get_num_children(AST*);
extern AST*  ast_get_child(AST*, size_t);
extern void ast_set_child(AST*, size_t index, AST* value);
extern int  ast_get_type(AST*);

#endif
