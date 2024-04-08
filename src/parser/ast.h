#ifndef AST_H
#define AST_H

#include <stdio.h>
#include "../include/util.h"
#include "../include/lexer.h"
#include "parser_shared.h"


typedef struct AST
{
  size_t size;
  size_t num_children;

  AST_Type type;
  size_t token_num;
  struct AST** children;

} AST;

string* ast_to_string(Arena*, AST*, Lexer*, int indent);

static AST zero = {
  0, 0, 0, 0, 0
};

#endif
