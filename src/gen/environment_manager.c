#include <defs.h>
#include <util/vector.h>
#include <util/hash_map.h>
#include <ast.h>
#include <environment_manager.h>

typedef string* string_ptr;
typedef Variable* Variable_ptr;

int string_compare(string* a, string* b) { return u_strcmp(a, b) == 0; }
hash_map_template(string_ptr, Variable_ptr)
hash_map_impl(string_ptr, Variable_ptr, u_string_hash, string_compare)

hash_map_template(string_ptr, size_t)
hash_map_impl(string_ptr, size_t, u_string_hash, string_compare)

typedef struct {
  HashMap(string_ptr, Variable_ptr)* variables;
  // HashMap(string_ptr, size_t)* scopes;
  size_t location;
} Environment;

typedef Environment* Environment_ptr;
hash_map_template(string_ptr, Environment_ptr)
hash_map_impl(string_ptr, Environment_ptr, u_string_hash, string_compare)

vector_template(Environment)
vector_impl(Environment, 10)

Arena* MEMORY;
HashMap(string_ptr, Environment_ptr) ENVIRONMENT_MANAGER;

void environment_manager_init()
{
  MEMORY = arena_create_init(MB(1));
  hash_map_init(string_ptr, Environment_ptr)(&ENVIRONMENT_MANAGER, MEMORY, 1000);
}

void environment_manager_create_environment(string* name)
{
  Environment* e = arena_alloc(MEMORY, sizeof(Environment));
  e->variables = hash_map_create(string_ptr, Variable_ptr)(MEMORY, 1024);
  e->location = 0;
  hash_map_add(string_ptr, Environment_ptr)(&ENVIRONMENT_MANAGER, name, e);
}

Environment* get_environment(string* env_name)
{
  optional(Environment_ptr) e_option = hash_map_get(string_ptr, Environment_ptr)(&ENVIRONMENT_MANAGER, env_name);
  if (e_option.result == NONE) die("error: environment_add_variable: can't find environment \"%s\"\n", env_name);
  return e_option.value;
}

string* type_ast_to_signature(AST* type)
{
  if (type->type != ASTType_TYPE) {
    die(ANSI_COLOR_RED "error: " ANSI_COLOR_RESET "type_ast_to_signature: AST is malformed!\n");
  }
  return u_strnew(MEMORY, "type");
}

void environment_add_variable(string* env_name, string* var_name, string* type_signature)
{
  Environment* e = get_environment(env_name);
  Variable* v = arena_alloc(MEMORY, sizeof(Variable));
  v->type_signature = u_strcpyar(MEMORY, type_signature);
  v->location = e->location;
  // const Token* t = lexer_get_token(var->lexer, var->token_line, var->token_idx);
  hash_map_add(string_ptr, Variable_ptr)(e->variables, u_strcpyar(MEMORY, var_name), v);
  e->location += 8;
}

Variable* environment_get_variable(string* env_name, string* var_name)
{
  Environment* e = get_environment(env_name);
  optional(Variable_ptr) v_option = hash_map_get(string_ptr, Variable_ptr)(e->variables, var_name);
  if (v_option.result == NONE) die("error: environment_add_variable: can't find environment \"%s\"\n", var_name);
  return v_option.value;
}

void environment_destroy()
{
  arena_free(MEMORY);
}
