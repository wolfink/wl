#ifndef TEST_H
#define TEST_H

#define NUM_COMMANDS 12
static char test_commands[NUM_COMMANDS][100] = {
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
  "f = e.a;"
};

#endif
