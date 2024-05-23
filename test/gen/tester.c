#include <defs.h>
#include <stdio.h>
#include <string.h>
#include <lexer.h>
#include <parser.h>

#include <gen.h>
#include <cfg.h>
#include <environment_manager.h>
#include "cfg_test.h"
#include "../test.h"

int test_cfg_solo()
{
  int failed = 0;

  for (int i = 0; i < NUM_COMMANDS(test_command_ids); i++)
  {
    Arena* a = arena_create();

    string* in = u_strnew(a, test_commands[test_command_ids[i]]);
    string* exp = u_strnew(a, test_commands_expected[i]);

    printf("Testing \"");
    u_prints(in);
    printf("\"...");

    Lexer* l = lexer_create(a, "N/A");
    lexer_scan(l, in);
    // u_prints(lexer_to_string(a, l));
    Parser* p = parser_create(a, l);
    AST* ast = parser_generate_ast(p);
    ControlFlowGraph* c = cfg_create(a);
    cfg_scan_ast(c, ast);
    string* out = cfg_to_string(a, c);

    if (u_strcmp(out, exp) != 0) {
      failed = 1;
      printf("FAILED!\n\nGot:\n");
      u_prints(out);
      printf("\nExpected:\n");
      u_prints(exp);
    } else printf("Passed\n");

    arena_free(a);
  }

  return failed;
}

int test_cfg_file(const char* filename)
{
  FILE* file = fopen(filename, "r");
  if (file == NULL) die(ANSI_COLOR_RED "error: " ANSI_COLOR_RESET "file not found\n");

  Arena* local = arena_create_init(MB(1));
  string* in = u_read_file(local, file);
  Lexer* l = lexer_create(local, filename);
  lexer_scan(l, in);
  Parser* p = parser_create(local, l);
  AST* tree = parser_generate_ast(p);
  ControlFlowGraph* cfg = cfg_create(local);
  cfg_scan_ast(cfg, tree);
  return 0;
}

AST* ast_get_var(AST* root)
{
  if (root->type == ASTType_VAR) return root;
  for(int i = 0; i < ast_get_num_children(root); i++) {
    AST* child = ast_get_var(ast_get_child(root, i));
    if (child) return child;
  }
  return NULL;
}

int test_environment_manager()
{
  Arena* a = arena_create();

  environment_manager_init();
  environment_manager_create_environment(u_strnew(a, "default"));

  for (int i = 0; i < 2; i++)
  {
    Arena* b = arena_create();

    string* in = u_strnew(b, test_commands[test_command_ids[i]]);
    string* exp = u_strnew(b, test_commands_expected[i]);

    Lexer* l = lexer_create(b, "N/A");
    lexer_scan(l, in);

    Parser* p = parser_create(b, l);
    AST* ast = parser_generate_ast(p);
    AST* var = ast_get_var(ast);
    if (var == NULL) continue;

    // environment_add_variable(u_strnew(b, "default"), var);
    arena_free(b);
  }
  return 0;
}

int main(int argc, char** argv)
{
  if (argc < 2) return 1;
  if (IS_ARG("cfg_solo")) return test_cfg_solo();
  else if(IS_ARG("envman")) return test_environment_manager();
  else if (IS_ARG("cfg_file")) return test_cfg_file(argv[2]);
}
