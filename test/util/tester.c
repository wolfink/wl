#include <util/arena.h>
#include <util/vector.h>
#include <util/core.h>
#include <util/string.h>
#include <util/hash_map.h>
#include <string.h>

typedef struct point { int x; int y; } point;
typedef point* point_ptr;

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

int test_vector_create()
{
  Arena* a = arena_create();
  vector_int* vi = vector_create(int)(a);
  vector_object* vs = vector_create(object)(a);
  vector_object* vp = vector_create(object)(a);
  if (vi->len != 0 || vs->len != 0 || vp->len != 0) return 1;
  if (vi->size != 100 || vs->size != 50 || vp->size != 50) return 2;
  if (vi->mem != a || vs->mem != a || vp->mem != a) return 3;
  int i = vi->values[0];
  string* s = vs->values[0];
  point* p = vp->values[0];
  return 0;
}


typedef string* string_ptr;
hash_set_template(string_ptr)
hash_map_template(point_ptr, string_ptr)

int test_vector_add()
{
  Arena* a = arena_create();
  vector_int* vi = vector_create(int)(a);
  for (int i = 0; i < 1000000; i++) vector_add(int)(vi, i);
  for (int i = 0; i < 1000000; i++) if (vi->values[i] != i) return 1;
  return 0;
}

int string_hash_map_compare(string* a, string* b)
{
  return u_strcmp(a, b) == 0;
}

// Always collides
size_t point_hash(point* s)
{
  return 0;
}

int point_cmp(point* a, point* b)
{
  return a->x == b->x && a->y == b->y;
}

hash_set_impl(string_ptr, u_string_hash, string_hash_map_compare)
hash_map_impl(point_ptr, string_ptr, point_hash, point_cmp)

int test_hash()
{

  char* c1 = "hello";
  char* c2 = "goodbye";
  char* c3 = "tell me why";

  Arena* a = arena_create();
  string* s1 = u_strnew(a, c1);
  string* s2 = u_strnew(a, c2);
  string* s3 = u_strnew(a, c3);
  string* s4 = u_strcat(a, u_strcat(a, s1, s2), s3);
  string* s5 = u_strcats(a, s4, ", oh my!");

  Arena* b = arena_create();
  HashSet(string_ptr)* hs = hash_set_create(string_ptr)(a, 1000);
  hash_set_add(string_ptr, hs, s1);
  if (!hash_set_contains(string_ptr)(hs, u_strcpyar(b, s1))) return 1;
  hash_set_add(string_ptr, hs, s2);
  if (!hash_set_contains(string_ptr)(hs, u_strcpyar(b, s2))) return 1;
  hash_set_add(string_ptr, hs, s3);
  if (!hash_set_contains(string_ptr)(hs, u_strcpyar(b, s3))) return 1;
  hash_set_add(string_ptr, hs, s4);
  if (!hash_set_contains(string_ptr)(hs, u_strcpyar(b, s4))) return 1;
  hash_set_add(string_ptr, hs, s5);
  if (!hash_set_contains(string_ptr)(hs, u_strcpyar(b, s5))) return 1;
  arena_free(a);
  arena_free(b);

  a = arena_create();

  // Test hash map collisions
  point p1 = {2, 1};
  point p2 = {0, 2};
  point p3 = {-2, 3};
  point p4 = {6, -1};

  point p1c = p1;
  point p2c = p2;
  point p3c = p3;
  point p4c = p4;

  HashMap(point_ptr, string_ptr)* hs2 = hash_map_create(point_ptr, string_ptr)(a, 1000);
  hash_map_add(point_ptr, string_ptr)(hs2, &p1, s1);
  hash_map_add(point_ptr, string_ptr)(hs2, &p2, s2);
  hash_map_add(point_ptr, string_ptr)(hs2, &p3, s3);
  hash_map_add(point_ptr, string_ptr)(hs2, &p4, s2);

  optional(string_ptr) opt;
  #define test_point(p, s)\
    opt = hash_map_get(point_ptr, string_ptr)(hs2, &p);\
    if (opt.result == NONE) return 1;\
    else if (u_strcmp(opt.value, s) != 0) return 2;\

  test_point(p1c, s1)
  test_point(p2c, s2)
  test_point(p3c, s3)
  test_point(p4c, s2)

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
  if (strcmp(argv[1], "vector_create") == 0) return test_vector_create();
  if (strcmp(argv[1], "vector_add") == 0) return test_vector_add();
  if (strcmp(argv[1], "hash") == 0) return test_hash();
  return 1;
}
