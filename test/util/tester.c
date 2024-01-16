#include <stdio.h>
#include <string.h>
#include "../../src/include/util.h"

int test_strcat()
{
  Arena* a = arena_create();
  char c1[100] = "";
  char c2[100] = "Hello";
  string* s1 = u_strnew(a, c1);
  string* s2 = u_strnew(a, c2);
  strcat(c1, c2);
  string* s3 = u_strnew(a, c1);
  s1 = u_strcat(a, s1, s2);
  if (u_strcmp(s1, s3) != 0) {
    printf("strcat failed! Got: ");
    u_prints(s1);
    printf(", Expected: ");
    u_prints(s3);
    printf("\n");
    return 1;
  }

  char c4[100] = ", world!\n";
  s2 = u_strnew(a, c4);
  strcat(c1, c4);
  s1 = u_strcat(a, s1, s2);
  s3 = u_strnew(a, c1);
  if (u_strcmp(s1, s3) != 0) {
    printf("strcat failed! Got: ");
    u_prints(s1);
    printf(", Expected: ");
    u_prints(s3);
    printf("\n");
    return 2;
  }
  return 0;
}

int test_strnew()
{
  Arena* a = arena_create();
  char *c = "";
  string* s = u_strnew(a, "");
  if (u_strlen(s) != strlen(c)) {
    printf("string of wrong size! Got: %ld, Expected: %ld\n", u_strlen(s), strlen(c));
    return 1;
  }

  c = "Hello, world!\n";
  s = u_strnew(a, c);
  if (u_strlen(s) != strlen(c)) {
    printf("string of wrong size! Got: %ld, Expected: %ld\n", u_strlen(s), strlen(c));
    return 2;
  }
  for (int i = 0; i < u_strlen(s); i++)
  {
    if(u_getc(s, i) != c[i]) {
      printf("wrong character at index %d, Got: %c, Expected: %c\n", i, u_getc(s, i), c[i]);
      return 3;
    }
  }
  return 0;
}

int test_arena_init()
{
  Arena* a = arena_create_init(1000000);
  if (arena_get_size(a) != 1000000) return 1;
  if (arena_alloc(a, 1500000) == NULL) return 2;
  return 0;
}

int test_arena_alloc()
{
  Arena* a = arena_create();
  if (arena_alloc(a, 2 << 8)  == NULL) return 1;
  printf("arena_size: %lu\n", arena_get_size(a));
  if (arena_alloc(a, 2 << 10) == NULL) return 2;
  printf("arena_size: %lu\n", arena_get_size(a));
  if (arena_alloc(a, 2 << 12) == NULL) return 3;
  printf("arena_size: %lu\n", arena_get_size(a));
  if (arena_alloc(a, 2 << 16) == NULL) return 4;
  printf("arena_size: %lu\n", arena_get_size(a));
  if (arena_alloc(a, 2 << 20) == NULL) return 5;
  printf("arena_size: %lu\n", arena_get_size(a));
  if (arena_alloc(a, 2 << 24) == NULL) return 6;
  printf("arena_size: %lu\n", arena_get_size(a));
  arena_free(a);
  return 0;
}

int main(int argc, char** argv)
{
  if (argc < 2) return 1;
  if (strcmp(argv[1], "arena_alloc") == 0) return test_arena_alloc();
  if (strcmp(argv[1], "arena_init") == 0) return test_arena_init();
  if (strcmp(argv[1], "strnew") == 0) return test_strnew();
  if (strcmp(argv[1], "strcat") == 0) return test_strcat();
  return 1;
}