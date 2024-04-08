#include<stdio.h>
#include<string.h>
#include<util.h>
#include<lexer.h>
#include<parser.h>

#include<gen.h>
#include"../../src/gen/cfg.h"
#include"cfg_test.h"

int test_cfg_solo()
{
  Arena* a = arena_create();
  int failed = 0;

  for (int i = 0; i < sizeof(test_command_ids) / sizeof(int); i++)
  {
    string* in = u_strnew(a, test_commands[test_command_ids[i]]);
    string* exp = u_strnew(a, test_commands_expected[i]);

    printf("Testing \"");
    u_prints(in);
    printf("\"...");

    Lexer* l = lexer_create(a, in);
    Parser* p = parser_create(a, l);
    ControlFlowGraph* c = cfg_create(a);
    cfg_scan_ast(c, parser_get_ast(p));
    string* out = cfg_to_string(c);

    if (u_strcmp(out, exp) != 0) {
      failed = 1;
      printf("FAILED!\n\nGot:\n");
      u_prints(out);
      printf("\nExpected:\n");
      u_prints(exp);
    } else printf("Passed\n");
  }

  return failed;
}

int main(int argc, char** argv)
{
  if (argc < 2) return 1;
  if (strcmp(argv[1], "cfg_solo") == 0) return test_cfg_solo();
}
