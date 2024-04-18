#include<stdio.h>
#include<string.h>
#include<util.h>
#include<lexer.h>
#include<parser.h>
#include"parser_test.h"
#include"../test.h"

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
    string* out = parser_to_string(a, p);
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

int main(int argc, char** argv)
{
  if (argc < 2) return 1;
  else if (strcmp(argv[1], "solo") == 0) return test_parser_solo();
  return 1;
}
