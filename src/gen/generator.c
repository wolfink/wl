#include <defs.h>
#include <parser.h>
#include <cfg.h>

#define WLC_VERSION "0.1.0"
#define WLC_DATE ""

typedef struct {
  string* filename;
  FILE* file;
  ControlFlowGraph* cfg;
} Generator;

void generate_globals(Generator* gen);
void generate_functions(Generator* gen);
void generate_start(Generator* gen);
void generate_instruction(Generator* gen, size_t node_no, size_t index);

Generator* generator_create(Arena* context, const char* filename)
{
  Generator* out = arena_alloc(context, sizeof(Generator));
  out->file = fopen(filename, "w");
  out->filename = u_strnew(context, filename);
  return out;
}

void generate(Generator* gen)
{
  NULL_CHECK(gen->file, generate)
  fprintf(gen->file,
          "\t.file\t\"%s\"\n"
          "\t.data\n", gen->filename);
  generate_globals(gen);
  fprintf(gen->file,
          "\t.text\n");
  generate_functions(gen);
  fprintf(gen->file,
          "\t.globl\t_start\n"
          "\t.type\t_start, @function\n"
          "_start:\n"
          "\txor\t%%rbp, %%rbp\n"
          "\tmovq\t%%rsp, %%rbp\n");
  generate_start(gen);
  fprintf(gen->file,
          "\tpushq\t%%rax\n"
          "_start_end:\n"
          "\t.size\t_start, .-_start\n"
          "\t.globl\t_exit\n"
          "\t.type\t_exit, @function\n"
          "_exit:\n"
          "\tmovq\t$60, %%rax\n"
          "\tmovq\t0(%%rsp), %%rdi\n"
          "\tsyscall\n"
          ".F0:\n"
          "\t.size\t_exit, .-_exit\n"
          "\t.ident\t\"WLC: v" WLC_VERSION WLC_DATE "\"\n");
}

void generate_globals(Generator* gen) 
{
  return;
}

void generate_functions(Generator* gen)
{
  return;
}

void generate_start(Generator* gen)
{
  return;
}

void generate_instruction(Generator* gen, size_t node_no, size_t index)
{
}
