#include <memory.h>
#include <util.h>
#include <defs.h>
#include "cfg.h"

#define CFG_DEFAULT_SIZE 10

ControlFlowGraph* cfg_create(Arena* context)
{
  ControlFlowGraph* cfg = arena_alloc(context, sizeof(ControlFlowGraph));
  cfg->memory = context;
  cfg->len  = 0;
  cfg->size = CFG_DEFAULT_SIZE;
  cfg->node_list = arena_alloc(context, sizeof(ControlFlowGraph*) * CFG_DEFAULT_SIZE);
  return cfg;
}

void cfg_scan_ast(ControlFlowGraph* c, AST* in)
{
  NULL_CHECK(c, cfg_scan_ast)

  ControlFlowGraphNode* main_node = cfgn_create(c->memory);
  cfg_add_node(c, main_node);
  cfgn_populate(main_node, in);
}

void cfg_resize(ControlFlowGraph* cfg)
{
  NULL_CHECK(cfg, cfg_resize)

  if (cfg->len >= cfg->size) cfg->size *= 2;
  ControlFlowGraphNode** new_node_list = arena_alloc(cfg->memory, sizeof(ControlFlowGraphNode*) * cfg->size);
  memcpy(new_node_list, cfg->node_list, cfg->len);
  cfg->node_list = new_node_list;
}

void cfg_add_node(ControlFlowGraph* cfg, ControlFlowGraphNode* n)
{
  NULL_CHECK(cfg, cfg_add_node)
  NULL_CHECK(n,   cfg_add_node)

  cfg_resize(cfg);
  cfg->node_list[cfg->len++] = n;
}

void cfg_handle_branch(
  Arena* context,
  ControlFlowGraph* cfg,
  AST* branch_ast,
  AST* next_ast)
{
  NULL_CHECK(cfg, cfg_handle_branch)

  if (ast_get_type(branch_ast) != ASTType_BRANCH) {
    die("error: cfg_handle_branch: expected BRANCH stmt in cfg_handle_branch\n");
  }

  if (ast_get_num_children(branch_ast) < 3) {
    die("error: cfg_handle_branch: too few children in BRANCH stmt AST");
  }

  const size_t cond_idx = cfg->len;
  const size_t then_idx = cond_idx + 1;
  const size_t else_idx = cond_idx + 2;
  const size_t next_idx = cond_idx + 3;

  ControlFlowGraphNode* cond_node = cfgn_create(context);
  ControlFlowGraphNode* then_node = cfgn_create(context);
  ControlFlowGraphNode* else_node = cfgn_create(context);
  ControlFlowGraphNode* next_node = cfgn_create(context);

  // Place child CFG's in control of parent code block's child code blocks
  cond_node->stmts = ast_get_child(branch_ast, 0);
  then_node->stmts = ast_get_child(branch_ast, 1);
  else_node->stmts = ast_get_child(branch_ast, 2);
  next_node->stmts = next_ast;

  if (ast_get_type(cond_node->stmts) != ASTType_EXPR
  || ast_get_type(then_node->stmts) != ASTType_BLOCK
  || ast_get_type(then_node->stmts) != ASTType_ELSE) {
    die("error: cfg_handle_branch: improperly formatted BRANCH stmt\n");
  }

  // End parent code block by NULL'ing it's children
  ast_set_child(cond_node->stmts, 1, NULL);
  ast_set_child(cond_node->stmts, 2, NULL);

  cfgn_add_connection(cond_node, then_idx);
  cfgn_add_connection(cond_node, else_idx);
  cfgn_add_connection(then_node, next_idx);
  cfgn_add_connection(else_node, next_idx);

  cfg_add_node(cfg, cond_node);
  cfg_add_node(cfg, then_node);
  cfg_add_node(cfg, else_node);
  cfg_add_node(cfg, next_node);
}

void cfg_handle_do(
  ControlFlowGraph* cfg,
  AST* do_ast,
  AST* next_ast
)
{
  NULL_CHECK(cfg,      cfg_handle_do)

  if (ast_get_type(do_ast) != ASTType_DO) {
    die("error: cfg_handle_do: expected DO stmt\n");
  }

  ControlFlowGraphNode* block_node = cfgn_create(cfg->memory);
  ControlFlowGraphNode* next_node  = cfgn_create(cfg->memory);

  const size_t block_idx = cfg->len;
  const size_t next_idx  = cfg->len + 1;

  cfgn_populate(block_node, do_ast);
}

void cfg_handle_while(
    Arena* context,
    ControlFlowGraph* cfg,
    AST* while_ast,
    AST* next_ast)
{
  NULL_CHECK(cfg, cfg_handle_while)

  if (ast_get_type(while_ast) != ASTType_WHILE) {
    die("error: cfg_handle_while: expected WHILE stmt\n");
  }

  ControlFlowGraphNode* cond_node = cfgn_create(cfg->memory);
  ControlFlowGraphNode* loop_node = cfgn_create(cfg->memory);
  ControlFlowGraphNode* next_node = cfgn_create(cfg->memory);

  const size_t cond_idx = cfg->len;
  const size_t loop_idx = cfg->len + 1;
  const size_t next_idx = cfg->len + 2;

  if (ast_get_num_children(while_ast) < 2) {
    die("error: cfg_handle_while: too few children in WHILE stmt AST\n");
  }

  cond_node->stmts = ast_get_child(while_ast, 0);
  loop_node->stmts = ast_get_child(while_ast, 1);
  next_node->stmts = next_ast;

  cfgn_add_connection(cond_node, loop_idx);
  cfgn_add_connection(loop_node, cond_idx);
  cfgn_add_connection(loop_node, next_idx);

  cfg_add_node(cfg, cond_node);
  cfg_add_node(cfg, loop_node);
  cfg_add_node(cfg, next_node);
}

string* cfg_to_string(ControlFlowGraph* c)
{
  string* ret = u_strnew(c->memory, "");
  return ret;
}

#define DEFAULT_CFGN_SIZE 2

ControlFlowGraphNode* cfgn_create(ControlFlowGraph* c) {
  NULL_CHECK(c, cfgn_create)

  ControlFlowGraphNode* ret = arena_alloc(c->memory, sizeof(ControlFlowGraphNode));
  ret->parent = c;
  ret->conn_len = 0;
  ret->conn_size = DEFAULT_CFGN_SIZE;
  ret->connection_list = arena_alloc(c->memory, sizeof(size_t) * DEFAULT_CFGN_SIZE);
  return ret;
}

void cfgn_resize_conn_list(ControlFlowGraphNode* n)
{
  NULL_CHECK(n, cfgn_resize_conn_list)

  if (n->conn_len >= n->conn_size) n->conn_size *= 2;
  size_t* new_cl = arena_alloc(n->parent->memory, sizeof(size_t) * n->conn_size);
  memcpy(new_cl, n->connection_list, n->conn_len);
  n->connection_list = new_cl;
}

void cfgn_resize_stmt_list(ControlFlowGraphNode* n)
{
  NULL_CHECK(n, cfgn_resize_stmt_list)

  if (n->stmt_len >= n->stmt_size) n->stmt_size *= 2;
  AST** new_sl = arena_alloc(n->parent->memory, sizeof(AST*) * n->stmt_size);
  memcpy(new_sl, n->stmts, n->stmt_len);
  n->stmts = new_sl;
}

void cfgn_add_connection(ControlFlowGraphNode* n, size_t index)
{
  NULL_CHECK(n, cfgn_add_connection)

  cfgn_resize_conn_list(n);
  n->connection_list[n->conn_len++] = index;
}

void cfgn_add_stmt(ControlFlowGraphNode* n, AST* stmt)
{
  NULL_CHECK(n, cfgn_add_stmt)

  if (ast_get_type(stmt) != ASTType_STMT) {
    die("error: cfgn_add_stmt: AST not of type STMT\n");
  }
  cfgn_resize_stmt_list(n);
  n->stmts[n->stmt_len++] = ast_get_child(stmt, 0);
}

void cfgn_populate(ControlFlowGraphNode* n, AST* in)
{
  NULL_CHECK(n, cfgn_populate)

  ControlFlowGraph* c = n->parent;
  Arena* context = c->memory;

  // Find STMT_LIST AST
  while (ast_get_num_children(in) > 0 && ast_get_type(in) != ASTType_STMT_LIST) {
    in = ast_get_child(in, 0);
  }
  ControlFlowGraphNode* main_node = cfgn_create(context);
  cfg_add_node(c, main_node);
  while (in)
  {
    if (ast_get_type(in) != ASTType_STMT_LIST) {
      die("error: cfgn_populate: bad AST format\n");
    }
    AST* stmt = ast_get_child(in, 0);
    AST* next = ast_get_child(in, 1);
    if (ast_get_type(stmt) != ASTType_STMT)
    {
      die("error: cfgn_populate: bad AST format\n");
    }

    AST* stmt_child = ast_get_child(stmt, 0);
    // If STMT does not manage control-flow, add it to the current CFG node
    switch (ast_get_type(stmt_child)) {
    case ASTType_BRANCH: 
      cfgn_add_connection(main_node, n->conn_len);
      cfg_handle_branch(context, c, stmt_child, next);
      main_node = c->node_list[c->len - 1];
      break;
    case ASTType_DO: case ASTType_CALL:
      // main_node->branches[main_node->conn_len] = stmt_cfg;
      // main_node->conn_len++;
      break;
    case ASTType_WHILE:
      cfgn_add_connection(main_node, n->conn_len);
      cfg_handle_while(context, c, stmt_child, next);
      main_node = c->node_list[c->len - 1];
      break;
    default:
      cfgn_add_stmt(main_node, stmt);
      break;
    }
    in = next;
  }
}
