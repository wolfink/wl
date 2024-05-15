#include <defs.h>
#include <stdio.h>
#include <string.h>
#include <lexer.h>
#include <parser.h>
#include "parser_test.h"
#include "../test.h"

int test_parser_solo()
{
  Arena* a = arena_create();
  int failed = 0;
  for (int i = 0; i < NUM_COMMANDS(test_command_ids); i++)
  {
    printf("Testing \"%s\"...", test_commands[test_command_ids[i]]);
    string* in = u_strnew(a, test_commands[test_command_ids[i]]);
    Lexer* l = lexer_create(a, in);
    Parser* p = parser_create(a, l);
    AST* ast = parser_generate_ast(p);
    string* out = ast_to_string(a, ast, 0);
    string* exp = u_strnew(a, test_commands_expected[i]);
    if (u_strcmp(out, exp) != 0) {
      failed = 1;
      printf("FAILED!\n\nGot:\n");
      u_prints(out);
      printf("\nExpected:\n");
      u_prints(exp);
      printf("\n");
    } else printf("Passed\n");
  }
  return failed;
}

int test_file(const char* filename)
{
  FILE* file = fopen(filename, "r");
  if (file == NULL) die(ANSI_COLOR_RED "error: " ANSI_COLOR_RESET "file not found\n");

  Arena* local = arena_create_init(MB(1));
  string* in = u_read_file(local, file);
  Lexer* l = lexer_create(local, filename);
  lexer_scan(l, in);
  Parser* p = parser_create(local, l);
  AST* tree = parser_generate_ast(p);
  return 0;
}

int main(int argc, char** argv)
{
  if (argc < 2) return 1;
  else if (IS_ARG("solo")) return test_parser_solo();
  else if (IS_ARG("file")) return test_file(argv[2]);
  return 1;
}
