#define AST_INTERNAL

#include <ast.h>
#include <memory.h>
#include <defs.h>

AST* ast_create(Arena* arena, ASTType type, Lexer* lex)
{
  AST* ret = arena_alloc(arena, sizeof(AST));
  ret->type = type;
  ret->lexer = lex;
  ret->num_children = 0;
  ret->size = 0;
  ret->token_idx = 0;
  ret->token_line = 0;
  return ret;
}

void ast_resize(Arena* arena, AST* ast, size_t new_size)
{
  NULL_CHECK(ast, ast_resize);

  if (new_size <= ast->size) return;
  AST** new_children = arena_alloc(arena, sizeof(AST*) * new_size);
  memcpy(new_children, ast->children, sizeof(AST*) * ast->size);
  ast->children = new_children;
  ast->size = new_size;
}

void ast_append(Arena* arena, AST* dest, AST* src)
{
  NULL_CHECK(src, ast_append);
  NULL_CHECK(dest, ast_append);

  if (!arena || !dest || !src) return;
  if (dest->size == 0) ast_resize(arena, dest, 1);
  else if (dest->size == 1) ast_resize(arena, dest, 3);
  else if (dest->num_children >= dest->size)  ast_resize(arena, dest, dest->size * 2);

  dest->children[dest->num_children] = src;
  dest->num_children++;
}

string* std_rule_to_string(Arena* context,
                           const AST* tree,
                           const string* src,
                           const string* val,
                           Lexer* lex,
                           int indent)
{
  NULL_CHECK(tree, std_rule_to_string)

  Arena* a = arena_create();

  string* ret;
  ret = u_strcat(a, src, val);
  ret = u_strcat(a, ret, u_strnew(context, "\n"));
  for(int i = 0; i < tree->num_children; i++)
  {
    string* child = ast_to_string(a, tree->children[i], indent + 1);
    ret = u_strcat(a, ret, child);
  } 
  ret = u_strcpyar(context, ret);
  arena_free(a);
  return ret;
}

string* list_rule_to_string(Arena* context,
                            const AST* tree,
                            const string* src,
                            const string* val,
                            Lexer* lex,
                            int indent)
{
  NULL_CHECK(tree,    list_rule_to_string)

  Arena* a = arena_create();

  string* ret;
  ret = u_strcat(a, src, val);
  ret = u_strcat(a, ret, u_strnew(a, "\n"));
  while(tree->num_children > 0) {
    ret = u_strcat(a, ret, ast_to_string(a, tree->children[0], indent + 1));
    if (tree->num_children < 2) break;
    tree = tree->children[1];
  }

  ret = u_strcpyar(context, ret);
  arena_free(a);
  return ret;
}

// Handle empty string errors
void ast_handle_str_error (Arena* a, AST* t, Lexer* l)
{
  NULL_CHECK(t, ast_handle_str_error)

  switch(t->type) {
#define X(name, string) \
  case ASTType_##name: \
    die("NULL string at type: %s", string);
  Rules
#undef X
  case ASTType_TOKEN:
#define X(name, string, first)
    die("NULL string at type: TOKEN, value: %s\n", lexer_get_token(l, t->token_line, t->token_idx)->value);
#undef X
  case ASTType_NUMBER:
#define X(name, string, first)
    die("NULL string at type: NUMBER, value: %s\n", lexer_get_token(l, t->token_line, t->token_idx)->value);
#undef X
  default:
    die("NULL string at type: ID\n");
  }
}

AST* ast_get_child(AST* tree, size_t index)
{
  NULL_CHECK(tree, ast_get_child)

  if (index >= tree->num_children) {
    die("error: ast_get_child: index %d is greater than or equal to number of children\n", index);
  }
  return tree->children[index];
}

void ast_set_child(AST* tree, size_t index, AST* value)
{
  NULL_CHECK(tree, ast_set_child)

  if (tree == NULL) {
    die("error: ast_get_type: NULL AST\n");
  }
  if (index > tree->num_children) {
    die("error: ast_set_child: index %d is greater than or equal to number of children\n", index);
  }
  tree->children[index] = value;
}

int ast_get_type(AST* tree)
{
  NULL_CHECK(tree, ast_get_type)
  return tree->type;
}

int  ast_is_token(AST* tree)
{
  NULL_CHECK(tree, ast_is_token)
  return tree->type == ASTType_TOKEN;
}

size_t ast_get_num_children(AST* tree)
{
  NULL_CHECK(tree, ast_get_num_children)
  return tree->num_children;
}

string* ast_to_string(Arena* context, AST* tree, int indent)
{
  NULL_CHECK(tree,    ast_to_string)

  Lexer* lex = tree->lexer;

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
    t = lexer_get_token(lex, tree->token_line, tree->token_idx)->type;
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
    case TokenType_ID: case TokenType_NUMBER: case TokenType_HEX: case TokenType_OCTAL: case TokenType_BINARY:
      ret = u_strcat(a, ret, u_strnew(a, ": "));
      ret = u_strcat(a, ret, lexer_get_token(lex, tree->token_line, tree->token_idx)->value);
      ret = u_strcat(a, ret, u_strnew(a, "\n"));
      break;
    default:
      ret = u_strcat(a, ret, u_strnew(a, "\n"));
      for(int i = 0; i < tree->num_children; i++)
      {
        ret = u_strcat(a, ret, ast_to_string(a, tree->children[i], indent + 1));
      }
      break;
    }
    break;
  default:
    break;
  }

  if (ret == NULL) ast_handle_str_error(context, tree, lex);

  ret = u_strcpyar(context, ret);
  arena_free(a);
  return ret;
}
