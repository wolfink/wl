#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <util.h>
#include <lexer.h>
#include "parser_shared.h"

typedef struct AST
{
  size_t size;
  size_t num_children;

  Lexer* lexer;
  ASTType type;
  size_t token_line;
  size_t token_idx;
  struct AST** children;

} AST;

string* ast_to_string(Arena*, AST*, int indent);

#endif
