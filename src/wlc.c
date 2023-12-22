#include "util.h"
#include "lexer.h"
#include "defs.h"
#include <stdio.h>
#include <stdlib.h>
#include <err.h>

int main(int argc, char** argv)
{
  Arena* a = arena_create_init(MB(1));
  arena_set_static(a, 1);

  if (argc < 2) {
    printf("Usage: wlc <filename>\n");
    return -1;
  }

  string* in = u_strnew(a, "");
  in = u_strcat(a, in, u_strnew(a, "x: int = 1000;"));
  in = u_strcat(a, in, u_strnew(a, "y: float = 0200;"));
  in = u_strcat(a, in, u_strnew(a, "z:(x: int, y: float)=>(ret: int);"));
  in = u_strcat(a, in, u_strnew(a, "w := 0x1adf;"));
  in = u_strcat(a, in, u_strnew(a, "a := 0b0011;"));
  Lexer* l = lexer_create(a, in);
  string* out = lexer_to_string(a, l);
  u_prints(out);
  printf("\n");
  for(int i = 0; i < lexer_get_len(l); i++)
  {
    Arena* b = arena_create();
    string* out = lexer_get_value_at_index(b, l, i);
    if (out) { u_prints(out); printf("\n"); }
    arena_free(b);
  }

  arena_free(a);
  return 0;
}
