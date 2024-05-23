#include <defs.h>
#include <parser.h>
#include <cfg.h>
#include <environment_manager.h>

#define WLC_VERSION "0.1.0"
#define WLC_DATE ""

#define RegisterTable\
  X(rax, eax, ax, al),\
  X(rbx, ebx, bx, bl),\
  X(rcx, ecx, cx, cl),\
  X(rdx, edx, dx, dl),\
  X(rsi, esi, si, sil),\
  X(rdi, edi, di, dil),\
  X(rbp, ebp, bp, bpl),\
  X(rsp, esp, sp, spl),\
  X(r8, r8d, r8w, r8b),\
  X(r9, r9d, r9w, r9b),\
  X(r10, r10d, r10w, r10b),\
  X(r11, r11d, r11w, r11b),\
  X(r12, r12d, r12w, r12b),\
  X(r13, r13d, r13w, r13b),\
  X(r14, r14d, r14w, r14b),\
  X(r15, r15d, r15w, r15b)\

enum {
#define X(quad, double, word, byte) quad
  RegisterTable,
#undef X
  NUM_REGISTERS,
  on_stack
} Register;

const char* Registers_str_64[] = {
#define X(quad, double, word, byte) #quad
  RegisterTable
#undef X
};

const char* Registers_str_32[] = {
#define X(quad, double, word, byte) #double
  RegisterTable
#undef X
};

const char* Registers_str_16[] = {
#define X(quad, double, word, byte) #word
  RegisterTable
#undef X
};

const char* Registers_str_8[] = {
#define X(quad, double, word, byte) #byte
  RegisterTable
#undef X
};

typedef struct {
  string* filename;
  FILE* file;
  string* environment;
  ControlFlowGraph* cfg;
  int registers[NUM_REGISTERS];
  size_t and_count;
  size_t or_count;
  size_t cond_count;
  // vector_int* stack;
  Arena* mem;
} Generator;

void generate_globals(Generator*);
void generate_functions(Generator*);
void generate_start(Generator*);
void generate_statement(Generator*, AST* stmt);
void generate_expr(Generator* gen, AST* tree, int result_register, string* label);
void generate_term(Generator* gen, AST* term, int result_register, string* label);

Generator* generator_create(Arena* context, const char* filename, ControlFlowGraph* c)
{
  Generator* out = arena_alloc(context, sizeof(Generator));
  out->file = fopen(filename, "w");
  out->filename = u_strnew(context, filename);
  out->cfg = c;
  out->environment = u_strnew(context, "public");
  out->mem = context;
  out->and_count = 0;
  out->or_count = 0;
  out->cond_count = 0;
  // out->stack = vector_create(int)(out->mem);
  return out;
}

void generate(Generator* gen)
{
  NULL_CHECK(gen->file, generate)
  fprintf(gen->file,
          "\t.file\t\"%s\"\n"
          "\t.data\n", gen->filename);
  // generate_globals(gen);
  fprintf(gen->file,
          "\t.text\n");
  // generate_functions(gen);
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

// Marks register full, moves value currently in register onto the stack
void generator_reserve_register(Generator* gen, int reg)
{
  if (gen->registers[reg]++) {
    // vector_add(int)(gen->stack, on_stack);
    fprintf(gen->file, "\tpushq\t%%%s\n", Registers_str_64[reg]);
  }
  // gen->registers[reg] = 1;
}

// Marks register empty
void generator_free_register(Generator* gen, int reg)
{
  gen->registers[reg] = 0;
}

void generator_fill_register(Generator* gen, int to)
{
  // int from = vector_int_pop(gen->stack);
  // if (from == on_stack) {
    // fprintf(gen->file, "\tpopq\t%%%s\n", Registers_str_64[to]);
  // } else {
    // fprintf(gen->file, "\tmovq\t%%%s, %%%s\n", Registers_str_64[from], Registers_str_64[to]);
  // }
}

void generate_start(Generator* gen) 
{
  size_t cfg_len = cfg_get_len(gen->cfg);
  for (int i = 0; i < cfg_len; i++) {
    ControlFlowGraphNode* node = cfg_get_node(gen->cfg, i);
    if (node->scope_no == 0) {
      for (int j = 0; j < node->stmts.len; j++) generate_statement(gen, node->stmts.values[j]);
    }
  }
  return;
}

void generate_functions(Generator* gen)
{
  return;
}

void generate_statement(Generator* gen, AST* stmt)
{
  generator_free_register(gen, rax);
  switch(stmt->children[0]->type) {
    case ASTType_EXPR: generate_expr(gen, stmt->children[0], rax, NULL); break;
    default: break;
  }
}

#define CHECK_AST(var, name, function) if (var->type != name) die(ANSI_COLOR_RED "error: " ANSI_COLOR_RESET #function ": wrong AST type!\n")

void generator_load_var(Generator* gen, AST* var, int reg)
{
  CHECK_AST(var, ASTType_VAR, generator_load_var);
  // Get variable data
  AST* id = var->children[0];
  const Token* id_t = lexer_get_token(id->lexer, id->token_line, id->token_idx);
  Variable* v = environment_get_variable(gen->environment, id_t->value);

  // Get variable name
  char var_name[u_strlen(id_t->value) + 1];
  strncpy(var_name, id_t->value, u_strlen(id_t->value));
  var_name[u_strlen(id_t->value)] = 0;

  // Reserve register and generate code
  if (reg == on_stack) {
    fprintf(gen->file, "\tpushq\t-%lu(%%rbp)\t\t# %s\n", v->location + 8, var_name);
  } else {
    fprintf(gen->file, "\tmovq\t-%lu(%%rbp), %%%s\t\t# %s\n", v->location + 8, Registers_str_64[reg], var_name);
  }
}

void generator_load_literal(Generator* gen, AST* literal, int reg)
{
  CHECK_AST(literal, ASTType_LITERAL, generator_load_literal);
  // Get literal string value
  AST* value = literal->children[0];
  const Token* value_t = lexer_get_token(value->lexer, value->token_line, value->token_idx);
  size_t str_len = u_strlen(value_t->value);
  char to_print[str_len + 1];
  strncpy(to_print, value_t->value, str_len);
  to_print[str_len] = '\0';

  // Reserve register and generate code
  if (reg == on_stack) {
    fprintf(gen->file, "\tpushq\t$%s\n", to_print);
  } else {
    fprintf(gen->file, "\tmovq\t$%s, %%%s\n", to_print, Registers_str_64[reg]);
  }
}

void generator_save_result(Generator* gen, int reg)
{
  if (reg == on_stack) {
    fprintf(gen->file, "\tpushq\t%%rax\n");
  } else if (reg != rax) {
    fprintf(gen->file, "\tmovq\t%%rax, %%%s\n", Registers_str_64[reg]);
  }
}

void generate_tuple(Generator* gen, AST* tuple, int result_register)
{
  CHECK_AST(tuple, ASTType_TUPLE, generate_tuple);
  AST* tuple_body = tuple->children[0];
  int num_children = ast_get_num_children(tuple_body);
  int reg = rax;
  // If tuple is a structure (has more than 1 child), place on stack
  // and place direct pointer into rax.
  if (num_children > 1) {
    fprintf(gen->file, "\tmovq\t%%rsp, %%rax\n");
    reg = on_stack;
  }
  for (int i = 0; i < num_children; i++) generate_expr(gen, tuple_body->children[0], reg, NULL);
  if (num_children == 1) generator_save_result(gen, result_register);
}

void generate_assignment(Generator* gen, AST* var, AST* expr)
{
  const Token* t = (var->type == ASTType_VAR)
    ? lexer_get_token(var->children[0]->lexer, var->children[0]->token_line, var->children[0]->token_idx)
    : lexer_get_token(var->lexer, var->token_line, var->token_idx);
  generate_expr(gen, expr, on_stack, NULL);
  environment_add_variable(gen->environment, (string*) t->value, u_strnew(gen->mem, "type"));
}

void generate_arithmetic_terms(Generator* gen, AST* tree, int result_register)
{
  int second_register = rsi;
  switch(tree->children[0]->type) {
  case ASTType_LITERAL:
  case ASTType_VAR:
    second_register = rdi;
  default: break;
  }
  for (size_t i = ast_get_num_children(tree) - 1; i >= 0; i -= 2) {
    AST *term = tree->children[i];
    int reg = on_stack;
    if (i == 0)
      reg = rax;
    if (i == 2)
      reg = second_register;
    generate_term(gen, term, reg, NULL);
    if (i == 0) break;
  }
  for (size_t i = 1; i < ast_get_num_children(tree) / 2 + 1; i++) {
    AST *op = tree->children[i * 2 - 1];
    const Token *op_t = lexer_get_token(op->lexer, op->token_line, op->token_idx);
    if (i > 1) {
      second_register = rdi;
      fprintf(gen->file, "\tpopq\t%%rdi\n");
    }
    switch(op_t->type) {
    case TokenType_PLUS: fprintf(gen->file, "\tadd\t%%%s, %%rax\n", Registers_str_64[second_register]); break;
    case TokenType_MINUS: fprintf(gen->file, "\tsub\t%%%s, %%rax\n", Registers_str_64[second_register]); break;
    case TokenType_STAR: fprintf(gen->file, "\tmul\t%%%s\n", Registers_str_64[second_register]); break;
    case TokenType_FSLASH: fprintf(gen->file,
                                   "\tmovq\t$0, %%rdx\n"
                                   "\tdiv\t%%%s\n", Registers_str_64[second_register]); break;
    case TokenType_BW_OR:
      fprintf(gen->file, "\tor\t%%%s, %%rax\n", Registers_str_64[second_register]); break;
    case TokenType_BW_AND:
      fprintf(gen->file, "\tand\t%%%s, %%rax\n", Registers_str_64[second_register]); break;
    default: die(ANSI_COLOR_RED "error: " ANSI_COLOR_RESET "generate_sum: malformed AST!\n");
    }
  }
  generator_save_result(gen, result_register);
}

void generate_relational_terms(Generator* gen, AST* tree, int result_register, string* label, int set_return)
{
  for (int i = 1; i < ast_get_num_children(tree); i += 2) {
    AST* op = tree->children[i]; if (op->type != ASTType_TOKEN) {
      die(ANSI_COLOR_RED "error: " ANSI_COLOR_RESET "generate_EXPR_REL: bad AST format!\n");
    }
    AST* term = tree->children[i + 1];
    int reg = (i == 0) ? r10: r11;
    const Token *t = lexer_get_token(term->lexer, term->token_idx, term->token_line);

    generate_term(gen, term, result_register, NULL);
    switch(t->type) {
    case TokenType_LANGLE:
      fprintf(gen->file,
              "\tcmp\t%%r10, %%r11\n"
              "\tjge\t%s\n", label);
      break;
    case TokenType_RANGLE:
      fprintf(gen->file,
              "\tcmp\t%%r10, %%r11\n"
              "\tjle\t%s\n", label);
      break;
    case TokenType_GT_EQUALS:
      fprintf(gen->file,
              "\tcmp\t%%r10, %%r11\n"
              "\tjl\t%s\n", label);
      break;
    case TokenType_LT_EQUALS:
      fprintf(gen->file,
              "\tcmp\t%%r10, %%r11\n"
              "\tjg\t%s\n", label);
      break;
    case TokenType_EQUALS:
      fprintf(gen->file,
              "\tcmp\t%%r10, %%r11\n"
              "\tjne\t%s\n", label);
    case TokenType_NOT_EQUALS:
      fprintf(gen->file,
              "\tcmp\t%%r10, %%r11\n"
              "\tje\t%s\n", label);
      break;
    default: break;
    }
    fprintf(gen->file, "\tmovq\t%%r11, %%r10\n");
  }
  if (set_return) {
    fprintf(gen->file, 
            "\tmovq\t$1, %%rax\n"
            "\tjmp\t.CE%lu\n"
            "%s:\n"
            "\tmovq\t$0, %%rax\n"
            ".CE%lu:\n", gen->cond_count, label, gen->cond_count);
    gen->cond_count++;
  }
  generator_save_result(gen, result_register);
}

void generate_and_terms(Generator* gen, AST* tree, int result_register, string* label, int set_return)
{
  for (int i = 0; i < ast_get_num_children(tree); i += 2) {
    generate_term(gen, tree->children[0], rax, NULL);
    fprintf(gen->file,
            "\ttest\t%%rax, %%rax\n"
            "\tjz\t%s\n", label);
  }
  if (set_return) {
    fprintf(gen->file,
            "\tmovq\t$1, %%rax\n"
            "\tjmp\t.CE%lu\n"
            "%s:\n"
            "\tmovq\t$0, %%rax\n"
            ".CE%lu:\n", gen->cond_count, label, gen->cond_count);
    gen->cond_count++;
    generator_save_result(gen, result_register);
  }
}

void generate_or_terms(Generator* gen, AST* tree, int result_register, string* label, int set_return)
{
  for (int i = 0; i < ast_get_num_children(tree); i += 2) {
    generate_term(gen, tree->children[0], rax, NULL);
    fprintf(gen->file,
            "\ttest\t%%rax, %%rax\n"
            "\tjnz\t%s\n", label);
  }
  if (set_return) {
    fprintf(gen->file,
            "\tmovq\t$0, %%rax\n"
            "\tjmp\t.CE%lu\n"
            "%s:\n"
            "\tmovq\t$1, %%rax\n"
            ".CE%lu:\n", gen->cond_count, label, gen->cond_count);
    gen->cond_count++;
    generator_save_result(gen, result_register);
  }
}

void generate_conditional_terms(Generator* gen, AST* tree, int result_register, string* label)
{
  int set_return = 0;
  if (label == NULL) {
    char format[20];
    sprintf(format, ".CJ%lu", gen->cond_count);
    label = u_strnnew(gen->mem, format, 20);
    set_return = 1;
  }
  switch(tree->type) {
  case ASTType_EXPR_REL:
  case ASTType_EXPR_EQ:
    generate_relational_terms(gen, tree, result_register, label, set_return);
    break;
  case ASTType_EXPR_AND: generate_and_terms(gen, tree, result_register, label, set_return); break;
  case ASTType_EXPR_OR: generate_or_terms(gen, tree, result_register, label, set_return); break;
  default: break;
  }
}

void generate_term(Generator* gen, AST* term, int result_register, string* label)
{
  switch(term->type) {
  case ASTType_EXPR_SUM:
  case ASTType_EXPR_MUL:
  case ASTType_EXPR_BW_OR:
  case ASTType_EXPR_BW_AND:
    generate_arithmetic_terms(gen, term, result_register);
    break;
  case ASTType_EXPR_OR:
  case ASTType_EXPR_AND:
  case ASTType_EXPR_EQ:
  case ASTType_EXPR_REL:
    generate_conditional_terms(gen, term, result_register, label);
    break;
  case ASTType_TUPLE: generate_tuple(gen, term, result_register); break;
  case ASTType_VAR: generator_load_var(gen, term, result_register); break;
  case ASTType_LITERAL: generator_load_literal(gen, term, result_register); break;
  default: break;
  }
}

void generate_expr(Generator* gen, AST* tree, int result_register, string* label)
{
  CHECK_AST(tree, ASTType_EXPR, generate_expr);
  AST* term = tree->children[0];
  if (term->type == ASTType_TOKEN) {
    const Token *t = lexer_get_token(tree->lexer, tree->token_line, tree->token_idx);
    switch(t->type) {
    case TokenType_ASSIGN: generate_assignment(gen, tree->children[1], tree->children[2]); break;
    case TokenType_COLON_ASSIGN: generate_assignment(gen, tree->children[1], tree->children[2]); break;
    default: break;
    }
    return;
  }
  generate_term(gen, term, result_register, label);
}
