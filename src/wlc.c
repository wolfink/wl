#include "util.h"
#include "lexer.h"
#include<stdio.h>
#include<stdlib.h>
#include<err.h>

int main(int argc, char** argv)
{
  Arena* a = arena_create();

  if (argc < 2) {
    printf("Usage: wlc <filename>\n");
    return -1;
  }

  string* in = u_strnew(a, "let x: int = 1000; let y: float = 2000; let z:(x: int, y: float)=>(ret: int);");
  string* out = lexer_analyze(a, in);
  u_prints(out);
  printf("\n");

  arena_free(a);
  return 0;
}
