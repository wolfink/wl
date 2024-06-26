#ifndef TEST_H
#define TEST_H

#define TOTAL_NUM_COMMANDS sizeof(test_commands) / sizeof(char*)
#define NUM_COMMANDS(test_list) sizeof(test_list) / sizeof(size_t)
static char* test_commands[] = {
  "x: int = 1000;",
  "y: float = 1.200;",
  "z:(x: int, y: float)=>(ret: int);",
  "if x > 3 && y < 2 { x = z(x, y); } else { x = 1; }",
  "x = x * 2 + 1000 - 30;",
  "w := 0x1adf;",
  "a := 0b0011;",
  "b := 0200;",
  "c := 0.300;",
  "d := .400;",
  "e : (a: int, b: int) = (1, 2);",
  "f = e.a;",
  "i := 0; while i < 64 { f(i); i = i + 1; }",
  "i: 1,2..64; f(i);",
  "macro: #->() = { #->write(\"x: int = 1000\"); }; macro#;"
};

#define IS_ARG(str) strcmp(argv[1], str) == 0


#endif
