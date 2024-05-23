#include <ast.h>
#include <util/string.h>

typedef struct {
  string* type_signature;
  size_t location;
} Variable;

void environment_manager_init();
void environment_manager_create_environment(string* name);
void environment_add_variable(string* env_name, string* var_name, string* type_signature);
Variable* environment_get_variable(string* env_name, string* var_name);
void environment_destroy();
