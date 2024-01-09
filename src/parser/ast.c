#include "ast.h"
#include "parser_shared.h"
#include <memory.h>

AST* ast_create(Arena* arena, AST_Type type)
{
  AST* ret = arena_alloc(arena, sizeof(AST));
  ret->type = type;
  ret->num_children = 0;
  ret->size = 0;
  ret->token_num = 0;
  return ret;
}

void ast_resize(Arena* arena, AST* ast, size_t new_size)
{
  if (new_size <= ast->size) return;
  AST** new_children = arena_alloc(arena, sizeof(AST*) * new_size);
  memcpy(new_children, ast->children, sizeof(AST*) * ast->size);
  ast->children = new_children;
  ast->size = new_size;
}

void ast_append(Arena* arena, AST* dest, AST* src)
{
  if (dest->size == 0) ast_resize(arena, dest, 1);
  else if (dest->size == 1) ast_resize(arena, dest, 3);
  else if (dest->num_children >= dest->size)  ast_resize(arena, dest, dest->size * 2);

  dest->children[dest->num_children] = src;
  dest->num_children++;
}

string* std_rule_to_string(Arena* context, const AST* tree, const string* src, const string* val, Lexer* lex, int indent)
{
  string* ret;
  ret = u_strcat(context, src, val);
  ret = u_strcat(context, ret, u_strnew(context, "\n"));
  for(int i = 0; i < tree->num_children; i++)
  {
    string* child = ast_to_string(context, tree->children[i], lex, indent + 1);
    //printf("=== Before ===\n");
    //u_prints(child);
    //printf("\nchild size: %lu, ret size: %lu\n", u_strlen(child), u_strlen(ret));
    ret = u_strcat(context, ret, child);
    //printf("=== After ===\n");
    //u_prints(ret);
    //printf("\nchild size: %lu, ret size: %lu\n", u_strlen(child), u_strlen(ret));
  } 
  return ret;
}

string* list_rule_to_string(Arena* context, const AST* tree, const string* src, const string* val, Lexer* lex, int indent)
{
  string* ret;
  ret = u_strcat(context, src, val);
  ret = u_strcat(context, ret, u_strnew(context, "\n"));
  do {
    ret = u_strcat(context, ret, ast_to_string(context, tree->children[0], lex, indent + 1));
    tree = tree->children[1];
  } while(tree->num_children > 1);
  return ret;
}

string* ast_to_string(Arena* context, AST* tree, Lexer* lex, int indent)
{
  Arena* a = arena_create();
  TokenType t;

  string* ret = u_strnew(a, "");
  for (int i = 0; i < indent; i++) ret = u_strcat(a, ret, u_strnew(a, "| ")); \

  switch(tree->type)
  {
#define X(name, str) \
  case ASTType_##name: \
    ret = std_rule_to_string(a, tree, ret, u_strnew(a, str), lex, indent); \
    break;
  StdRules
#undef X
#define X(name, str) \
  case ASTType_##name: \
    ret = list_rule_to_string(a, tree, ret, u_strnew(a, str), lex, indent); \
    break;
  ListRules
#undef X
  case ASTType_TOKEN:
    t = lexer_get_token_type_at_index(lex, tree->token_num);
    switch(t) {
#define X(name, first, str) \
    case TokenType_##name: \
      ret = u_strcat(a, ret, u_strnew(a, str)); \
      break;
    TokenTypeTable
    default: break;
#undef X
    }
    switch(t)
    {
    case TokenType_ID: case TokenType_NUMBER:
      ret = u_strcat(a, ret, u_strnew(a, ": "));
      ret = u_strcat(a, ret, lexer_get_value_at_index(a, lex, tree->token_num));
      ret = u_strcat(a, ret, u_strnew(a, "\n"));
      break;
    default:
      ret = u_strcat(a, ret, u_strnew(a, "\n"));
      for(int i = 0; i < tree->num_children; i++)
      {
        ret = u_strcat(a, ret, ast_to_string(a, tree->children[i], lex, indent + 1));
      }
      break;
    }
    break;
  }

  arena_free(a);
  return u_strcpyar(context, ret);
}

int  ast_is_token(AST* ast)
{
  return ast->type == ASTType_TOKEN;
}
