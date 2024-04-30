#include <stdio.h>
#include <err.h>
#include <string.h>
#include <util.h>
#include <parser.h>
#include <lexer.h>
#include <defs.h>

#define SHOW_LEXER  1
#define SHOW_VALUES 1 << 1
#define SHOW_PARSER 1 << 2

void usage()
{
  die("Usage: wlc <filename> [-l lexer] [-v values] [-p parser]\n");
}

int main(int argc, char** argv)
{
  Arena* a = arena_create_init(MB(1));
  size_t flags = 0;
  //arena_set_static(a, 1);

  if (argc < 2) {
    usage();
  }

  for(int i = 2; i < argc; i++)
  {
    if (strcmp(argv[i], "-l") == 0)      flags |= SHOW_LEXER;
    else if (strcmp(argv[i], "-v") == 0) flags |= SHOW_VALUES;
    else if (strcmp(argv[i], "-p") == 0) flags |= SHOW_PARSER;
    else  usage();
  }

  FILE* fp = fopen(argv[1], "r");
  string* in = u_read_file(a, fp);

  Lexer* l = lexer_create(a, argv[1]);
  const int l_fail = lexer_scan(l, in);
  if (l_fail) die("%s:" ANSI_COLOR_RED " error: " ANSI_COLOR_RESET "lexer failed to read file\n", argv[1]);
  if (flags & SHOW_LEXER) {
    printf("== Lexer output ============\n\n");
    string* out = lexer_to_string(a, l);
    u_prints(out);
    printf("\n\n== End Lexer output ========\n\n");
  }

  if (flags & SHOW_VALUES) {
    printf("\n\n== Lexer values ============\n\n");
    for(int i = 0; i < lexer_get_num_lines(l); i++)
    {
      for (int j = 0; j < lexer_get_line_len(l, i); j++) {
        Arena* b = arena_create();
        printf("(%d", i);
        const string* out = lexer_get_token(l, i, j)->value;
        if (out) { printf(","); u_prints(out); printf(")"); }
        else printf(")");
        arena_free(b);
      }
    }
    printf("\n\n== End Lexer values ========\n\n");
  }

  Parser* p = parser_create(a, l);
  AST* ast = parser_generate_ast(p);
  if (flags & SHOW_PARSER) {
    printf("\n\n== Parser ouput ============\n\n");
    string* out = ast_to_string(a, ast, 0);
    u_prints(out);
    printf("\n== End Parser output========\n\n");
  }

  arena_free(a);
  return 0;
}
