#include <stdio.h>
#include <string.h>
#include "lexer_test.h"

#define IS_ARG(str) strcmp(argv[1], str) == 0

int test_token(TokenType type, const char* cstr)
{
  printf("Testing \"%s\"...", cstr);

  Arena* a = arena_create();
  string* in = u_strnew(a, cstr);
  Lexer *l = lexer_create(a, in);
  TokenType t = lexer_get_token_type_at_index(l, 0);
  TokenType t_e = type;


  if (t == t_e) {
    printf("Passed\n");
    return 0;
  } else {
    printf("FAILED! Got: ");
    u_prints(token_type_tostr(a, t));
    printf("  Expected: ");
    u_prints(token_type_tostr(a, t_e));
    printf("\n");
    return 1;
  }
  arena_free(a);
}

int test_tokens()
{
  printf("Running test_tokens()...\n");
  int ret = 0;
#define X(type, str) ret |= test_token(TokenType_##type, str);
  TokenTable
#undef X
  return ret;
}

int test_commands_solo()
{
  printf("Running test_commands_solo()\n");
  for(int i = 0; i < NUM_COMMANDS; i++)
  {
    printf("(%d/%d) ", i+1, NUM_COMMANDS);
    printf("Testing command: \"%s\"...", test_commands[i]);
    Arena* a = arena_create();
    string* in = u_strnew(a, test_commands[i]);
    Lexer* l = lexer_create(a, in);
    for (int j = 0; j < lexer_get_len(l); j++)
    {
      const TokenType t = lexer_get_token_type_at_index(l, j);
      const TokenType t_expected = test_commands_expected[i][j].t;
      const string* val = lexer_get_value_at_index(a, l, j);
      const string* val_expected = u_strnew(a, test_commands_expected[i][j].val);
      if (t != t_expected
          || (u_strlen(val_expected) > 0 && u_strcmp(val, val_expected) != 0)) {
        printf("Expected: ");
        u_prints(token_type_tostr(a, t_expected));
        printf(", Got: ");
        u_prints(token_type_tostr(a, t));
        printf("\n");
        return 1;
      }
    }
    printf("Passed\n");
    arena_free(a);
  }
  return 0;
}

int main(int argc, char** argv)
{
  if (argc < 2) return 1;
  if (IS_ARG("token")) return test_tokens();
  if (IS_ARG("solo")) return test_commands_solo();
  return 0;
}
