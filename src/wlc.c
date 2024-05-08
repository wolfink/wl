#include <stdio.h>
#include <err.h>
#include <string.h>
#include <util.h>
#include <parser.h>
#include <cfg.h>
#include <gen.h>
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

  string* load_path = NULL;

  for(int i = 2; i < argc; i++)
  {
#define IS_ARG(arg) strcmp(argv[i], arg) == 0
    if (IS_ARG("-l")) flags |= SHOW_LEXER;
    else if (IS_ARG("-v")) flags |= SHOW_VALUES;
    else if (IS_ARG("-p")) flags |= SHOW_PARSER;
    else if (IS_ARG("--load-path=")) load_path = u_strnew(a, argv[2]);
    else  usage();
  }
  if (load_path == NULL) load_path = u_strnew(a, "/usr/lib/wl/");

  FILE* fp = fopen(argv[1], "r");
  string* in = u_read_file(a, fp);

  Lexer* l = lexer_create(a, argv[1]);
  const int l_fail = lexer_scan(l, in);
  if (l_fail) die("%s:" ANSI_COLOR_RED " error: " ANSI_COLOR_RESET "lexer failed to read file\n", argv[1]);
  if (flags & SHOW_LEXER) {
    Arena* b = arena_create();
    printf("== Lexer output ============\n\n");
    string* out = lexer_to_string(b, l);
    u_prints(out);
    printf("\n\n== End Lexer output ========\n\n");
    arena_free(b);
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
    Arena* b = arena_create();
    printf("\n\n== Parser ouput ============\n\n");
    string* out = ast_to_string(b, ast, 0);
    u_prints(out);
    printf("\n== End Parser output========\n\n");
    arena_free(b);
  }

  ControlFlowGraph* c = cfg_create(a);
  cfg_scan_ast(c, p);
  if (flags & SHOW_PARSER) {
    Arena* b = arena_create();
    printf("\n\n== CFG output ==============\n\n");
    string* out = cfg_to_string(b, c);
    u_prints(out);
    printf("\n== End CFG output ==========\n\n");
    arena_free(b);
  }

  Generator* g = generator_create(a, "a.s");
  generate(g);

  arena_free(a);
  return 0;
}
