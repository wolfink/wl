#include <memory.h>
#include <util/vector.h>
#include <defs.h>
#include <stdio.h>
#include <parser.h>

struct CFG;

typedef struct {
  vector_object stmts;
  vector_size_t connection_list;
  struct CFG* parent;
  size_t environment;
  int generated;
} ControlFlowGraphNode;

typedef struct CFG {
  vector_object node_list;
  vector_size_t functions;
  Arena* memory;
} ControlFlowGraph;

void cfg_populate_node(ControlFlowGraph*, ControlFlowGraphNode*, AST*);
size_t cfg_handle_branch(ControlFlowGraph*, AST* branch_stmt);
size_t cfg_handle_do(ControlFlowGraph*, AST* do_stmt);
size_t cfg_handle_while(ControlFlowGraph*, AST* while_stmt);
void cfg_add_node(ControlFlowGraph*, ControlFlowGraphNode*);

ControlFlowGraphNode* cfgn_create(ControlFlowGraph*);
void cfgn_add_connection(ControlFlowGraphNode* n, size_t index);
void cfgn_add_stmt(ControlFlowGraphNode* n, AST* stmt);
string* cfgn_to_string(Arena* context, ControlFlowGraphNode*, size_t index);

ControlFlowGraph* cfg_create(Arena* context)
{
  ControlFlowGraph* cfg = arena_alloc(context, sizeof(ControlFlowGraph));
  cfg->memory = context;
  vector_object_init(cfg->memory, (vector_object*) cfg);
  vector_size_t_init(cfg->memory, &cfg->functions);
  return cfg;
}

AST* cfg_get_stmt(ControlFlowGraph* c, size_t node_no, size_t index)
{
  ControlFlowGraphNode* node = c->node_list.values[node_no];
  return node->stmts.values[index];
}

void cfg_scan_ast(ControlFlowGraph* c, AST* in)
{
  NULL_CHECK(c, cfg_scan_ast)

  ControlFlowGraphNode* main_node = cfgn_create(c);
  cfg_populate_node(c, main_node, in);
}

void cfg_populate_node(ControlFlowGraph* c, ControlFlowGraphNode* n, AST* in)
{
  NULL_CHECK(c, cfg_populate_node)
  NULL_CHECK(n, cfg_populate_node)

  Arena* context = c->memory;

  // Find STMT_LIST AST
  while (ast_get_num_children(in) > 0 && ast_get_type(in) != ASTType_STMT_LIST) {
    in = ast_get_child(in, 0);
  }
  while (ast_get_num_children(in) >= 2)
  {
    if (ast_get_type(in) != ASTType_STMT_LIST) {
      die("error: cfg_populate_node: bad AST format\n");
    }
    AST* stmt = ast_get_child(in, 0);
    AST* next = ast_get_child(in, 1);

    // If STMT does not manage control-flow, add it to the current CFG node
    AST* stmt_body = ast_get_child(stmt, 0);
    switch (ast_get_type(stmt_body)) {
    case ASTType_BRANCH: {
      const size_t cond_branch = c->node_list.len;
      cfgn_add_connection(n, cond_branch);

      const size_t next_branch = cfg_handle_branch(c, stmt_body);
      n = c->node_list.values[next_branch]; // Switch to continue branch
    } break;
    case ASTType_DO: case ASTType_CALL: {
    } break;
    case ASTType_WHILE: {
      const size_t cond_branch = c->node_list.len; 
      cfgn_add_connection(n, cond_branch);

      const size_t next_branch = cfg_handle_while(c, stmt_body);
      n = c->node_list.values[next_branch];
    } break;
    default:
      if (n->stmts.len == 0) cfg_add_node(c, n);
      cfgn_add_stmt(n, stmt);
    }
    in = next;
  }
}

void cfg_add_node(ControlFlowGraph* cfg, ControlFlowGraphNode* n)
{
  NULL_CHECK(cfg, cfg_add_node)
  NULL_CHECK(n,   cfg_add_node)

  vector_add(object)((vector_object*) cfg, n);
}

// Returns: index of the continuing branch
size_t cfg_handle_branch(ControlFlowGraph* cfg, AST* branch_ast)
{
  NULL_CHECK(cfg, cfg_handle_branch)

  if (ast_get_type(branch_ast) != ASTType_BRANCH) {
    die("error: cfg_handle_branch: expected BRANCH stmt in cfg_handle_branch\n");
  }

  if (ast_get_num_children(branch_ast) < 3) {
    die("error: cfg_handle_branch: too few children in BRANCH stmt AST");
  }

  const size_t cond_idx = cfg->node_list.len;
  const size_t then_idx = cond_idx + 1;
  const size_t else_idx = cond_idx + 2;
  const size_t next_idx = cond_idx + 3;

  ControlFlowGraphNode* cond_node = cfgn_create(cfg);
  ControlFlowGraphNode* then_node = cfgn_create(cfg);
  ControlFlowGraphNode* else_node = cfgn_create(cfg);
  ControlFlowGraphNode* next_node = cfgn_create(cfg);

  // Place child CFG's in control of parent code block's child code blocks
  cfg_add_node(cfg, cond_node);
  cond_node->stmts.values[0] = ast_get_child(branch_ast, 0);
  cond_node->stmts.len = 1;
  cfg_populate_node(cfg, then_node, ast_get_child(branch_ast, 1));
  cfg_populate_node(cfg, else_node, ast_get_child(branch_ast, 2));
  cfg_add_node(cfg, next_node);

  cfgn_add_connection(cond_node, then_idx);
  cfgn_add_connection(cond_node, else_idx);
  cfgn_add_connection(then_node, next_idx);
  cfgn_add_connection(else_node, next_idx);


  // if (ast_get_type(cond_node->stmts) != ASTType_EXPR
  // || ast_get_type(then_node->stmts) != ASTType_BLOCK
  // || ast_get_type(then_node->stmts) != ASTType_ELSE) {
  //   die("error: cfg_handle_branch: improperly formatted BRANCH stmt\n");
  // }

  // End parent code block by NULL'ing it's children
  // ast_set_child(cond_node->stmts, 1, NULL);
  // ast_set_child(cond_node->stmts, 2, NULL);


  return next_idx;
}

size_t cfg_handle_do(ControlFlowGraph* cfg, AST* do_ast)
{
  NULL_CHECK(cfg,      cfg_handle_do)

  if (ast_get_type(do_ast) != ASTType_DO) {
    die("error: cfg_handle_do: expected DO stmt\n");
  }

  ControlFlowGraphNode* block_node = cfgn_create(cfg);
  ControlFlowGraphNode* next_node  = cfgn_create(cfg);

  const size_t block_idx = cfg->node_list.len;
  const size_t next_idx  = cfg->node_list.len + 1;

  cfg_populate_node(cfg, block_node, do_ast);
  return next_idx;
}


size_t cfg_handle_while(ControlFlowGraph* cfg, AST* while_ast)
{
  NULL_CHECK(cfg, cfg_handle_while)

  if (ast_get_type(while_ast) != ASTType_WHILE) {
    die("error: cfg_handle_while: expected WHILE stmt\n");
  }

  ControlFlowGraphNode* cond_node = cfgn_create(cfg);
  ControlFlowGraphNode* loop_node = cfgn_create(cfg);
  ControlFlowGraphNode* next_node = cfgn_create(cfg);

  const size_t cond_idx = cfg->node_list.len;
  const size_t loop_idx = cfg->node_list.len + 1;
  const size_t next_idx = cfg->node_list.len + 2;

  if (ast_get_num_children(while_ast) < 2) {
    die("error: cfg_handle_while: too few children in WHILE stmt AST\n");
  }

  cfg_add_node(cfg, cond_node);
  cond_node->stmts.values[0] = ast_get_child(while_ast, 0);
  cond_node->stmts.len = 1;
  cfg_populate_node(cfg, loop_node, ast_get_child(while_ast, 1));
  cfg_add_node(cfg, next_node);

  cfgn_add_connection(cond_node, loop_idx);
  cfgn_add_connection(loop_node, cond_idx);
  cfgn_add_connection(cond_node, next_idx);

  return next_idx;
}

string* cfg_to_string(Arena* context, ControlFlowGraph* c)
{
  Arena* local = arena_create();
  string* ret = u_strnew(local, "");

  for (size_t node = 0; node < c->node_list.len; node++)
  {
    ret = u_strcat(local, ret, cfgn_to_string(local, c->node_list.values[node], node));
  }
  ret = u_strcpyar(context, ret);

  arena_free(local);
  return ret;
}

ControlFlowGraphNode* cfgn_create(ControlFlowGraph* c) {
  NULL_CHECK(c, cfgn_create)

  ControlFlowGraphNode* out = arena_alloc(c->memory, sizeof(ControlFlowGraphNode));
  out->parent = c;
  vector_object_init(c->memory, (vector_object*) out);
  vector_size_t_init(c->memory, &out->connection_list);
  out->generated = 0;
  return out;
}

void cfgn_add_connection(ControlFlowGraphNode* n, size_t index)
{
  NULL_CHECK(n, cfgn_add_connection)

  vector_size_t_add(&n->connection_list, index);
}

void cfgn_add_stmt(ControlFlowGraphNode* n, AST* stmt)
{
  NULL_CHECK(n, cfgn_add_stmt)

  if (ast_get_type(stmt) != ASTType_STMT) {
    die("error: cfgn_add_stmt: AST not of type STMT\n");
  }

  vector_object_add((vector_object*) n, stmt);
}


int cfgn_is_eof(ControlFlowGraphNode* n) { return n->stmts.len == 0; }

string* cfgn_to_string(Arena* context, ControlFlowGraphNode* n, size_t index)
{
  Arena* local = arena_create();

  char node_header_chars[n->connection_list.len * 21 + 9];
  char* node_header_ptr = node_header_chars;
  string* ret;

  node_header_ptr += sprintf(node_header_chars, "%lu => {", index);
  if (n->connection_list.len == 0) strcat(node_header_ptr, "EOF}:\n");
  for (int conn = 0; conn < n->connection_list.len; conn++)
  {
    char* fmt;
    if (conn == n->connection_list.len - 1) fmt = "%lu}:\n";
    else fmt = "%lu, ";
    node_header_ptr += snprintf(node_header_ptr, 21, fmt, n->connection_list.values[conn]);
  }
  ret = u_strnew(local, node_header_chars);

  if (n->stmts.len > 0) {
    Arena* body = arena_create();
    string* node_body = ast_to_string(body, n->stmts.values[0], 1);

    for (int stmt = 1; stmt < n->stmts.len; stmt++) {
      node_body = u_strcat(body, node_body, ast_to_string(local, n->stmts.values[stmt], 1));
    }
    ret = u_strcat(context, ret, node_body);

    arena_free(body);
  }

  arena_free(local);
  return ret;
}
