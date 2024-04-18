#include <memory.h>
#include <util.h>
#include <defs.h>
#include <stdio.h>
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
      const size_t cond_branch = c->len;
      cfgn_add_connection(n, cond_branch);

      const size_t next_branch = cfg_handle_branch(c, stmt_body);
      n = c->node_list[next_branch]; // Switch to continue branch
    } break;
    case ASTType_DO: case ASTType_CALL: {
    } break;
    case ASTType_WHILE: {
      const size_t cond_branch = c->len; 
      cfgn_add_connection(n, cond_branch);

      const size_t next_branch = cfg_handle_while(c, stmt_body);
      n = c->node_list[next_branch];
    } break;
    default:
      if (n->stmt_len == 0) cfg_add_node(c, n);
      cfgn_add_stmt(n, stmt);
    }
    in = next;
  }
}

void cfg_resize(ControlFlowGraph* cfg)
{
  NULL_CHECK(cfg, cfg_resize)

  if (cfg->len >= cfg->size) {
    cfg->size *= 2;

    size_t node_size = sizeof(ControlFlowGraphNode*);
    ControlFlowGraphNode** new_node_list = arena_alloc(cfg->memory, node_size * cfg->size);

    memcpy(new_node_list, cfg->node_list, node_size * cfg->len);
    cfg->node_list = new_node_list;
  }
}

void cfg_add_node(ControlFlowGraph* cfg, ControlFlowGraphNode* n)
{
  NULL_CHECK(cfg, cfg_add_node)
  NULL_CHECK(n,   cfg_add_node)

  cfg_resize(cfg);
  cfg->node_list[cfg->len++] = n;
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

  const size_t cond_idx = cfg->len;
  const size_t then_idx = cond_idx + 1;
  const size_t else_idx = cond_idx + 2;
  const size_t next_idx = cond_idx + 3;

  ControlFlowGraphNode* cond_node = cfgn_create(cfg);
  ControlFlowGraphNode* then_node = cfgn_create(cfg);
  ControlFlowGraphNode* else_node = cfgn_create(cfg);
  ControlFlowGraphNode* next_node = cfgn_create(cfg);

  // Place child CFG's in control of parent code block's child code blocks
  cfg_add_node(cfg, cond_node);
  cond_node->stmts[0] = ast_get_child(branch_ast, 0);
  cond_node->stmt_len = 1;
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

  const size_t block_idx = cfg->len;
  const size_t next_idx  = cfg->len + 1;

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

  const size_t cond_idx = cfg->len;
  const size_t loop_idx = cfg->len + 1;
  const size_t next_idx = cfg->len + 2;

  if (ast_get_num_children(while_ast) < 2) {
    die("error: cfg_handle_while: too few children in WHILE stmt AST\n");
  }

  cfg_add_node(cfg, cond_node);
  cond_node->stmts[0] = ast_get_child(while_ast, 0);
  cond_node->stmt_len = 1;
  cfg_populate_node(cfg, loop_node, ast_get_child(while_ast, 1));
  cfg_add_node(cfg, next_node);

  cfgn_add_connection(cond_node, loop_idx);
  cfgn_add_connection(loop_node, cond_idx);
  cfgn_add_connection(cond_node, next_idx);

  return next_idx;
}

string* cfg_to_string(Arena* context, ControlFlowGraph* c, Lexer* lex)
{
  Arena* local = arena_create();
  string* ret = u_strnew(local, "");

  for (size_t node = 0; node < c->len; node++)
  {
    ret = u_strcat(local, ret, cfgn_to_string(local, c->node_list[node], lex, node));
  }
  ret = u_strcpyar(context, ret);

  arena_free(local);
  return ret;
}

#define DEFAULT_CFGN_CONN_SIZE 2
#define DEFAULT_CFGN_STMT_SIZE 10

ControlFlowGraphNode* cfgn_create(ControlFlowGraph* c) {
  NULL_CHECK(c, cfgn_create)

  ControlFlowGraphNode* ret = arena_alloc(c->memory, sizeof(ControlFlowGraphNode));
  ret->parent = c;
  ret->conn_len = 0;
  ret->conn_size = DEFAULT_CFGN_CONN_SIZE;
  ret->connection_list = arena_alloc(c->memory, sizeof(size_t) * DEFAULT_CFGN_CONN_SIZE);
  ret->stmt_len = 0;
  ret->stmt_size = DEFAULT_CFGN_STMT_SIZE;
  ret->stmts = arena_alloc(c->memory, sizeof(size_t) * DEFAULT_CFGN_STMT_SIZE);
  return ret;
}

void cfgn_resize_conn_list(ControlFlowGraphNode* n)
{
  NULL_CHECK(n, cfgn_resize_conn_list)

  if (n->conn_len >= n->conn_size) {
    n->conn_size *= 2;

    size_t conn_size = sizeof(size_t);
    size_t* new_cl = arena_alloc(n->parent->memory, conn_size * n->conn_size);

    memcpy(new_cl, n->connection_list, conn_size * n->conn_len);
    n->connection_list = new_cl;
  }
}

void cfgn_resize_stmt_list(ControlFlowGraphNode* n)
{
  NULL_CHECK(n, cfgn_resize_stmt_list)

  if (n->stmt_len >= n->stmt_size) {
    n->stmt_size *= 2;

    size_t stmt_byte_size = sizeof(AST*);
    AST** new_sl = arena_alloc(n->parent->memory, stmt_byte_size * n->stmt_size);

    memcpy(new_sl, n->stmts, stmt_byte_size * n->stmt_len);
    n->stmts = new_sl;
  }
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


int cfgn_is_eof(ControlFlowGraphNode* n) { return n->stmt_len == 0; }

string* cfgn_to_string(Arena* context, ControlFlowGraphNode* n, Lexer* lex, size_t index)
{
  Arena* local = arena_create();

  char node_header_chars[n->conn_len * 21 + 9];
  char* node_header_ptr = node_header_chars;
  string* ret;

  node_header_ptr += sprintf(node_header_chars, "%lu => {", index);
  if (n->conn_len == 0) strcat(node_header_ptr, "EOF}:\n");
  for (int conn = 0; conn < n->conn_len; conn++)
  {
    char* fmt;
    if (conn == n->conn_len - 1) fmt = "%lu}:\n";
    else fmt = "%lu, ";
    node_header_ptr += snprintf(node_header_ptr, 21, fmt, n->connection_list[conn]);
  }
  ret = u_strnew(local, node_header_chars);

  if (n->stmt_len > 0) {
    Arena* body = arena_create();
    string* node_body = ast_to_string(body, n->stmts[0], lex, 1);

    for (int stmt = 1; stmt < n->stmt_len; stmt++) {
      node_body = u_strcat(body, node_body, ast_to_string(local, n->stmts[stmt], lex, 1));
    }
    ret = u_strcat(context, ret, node_body);

    arena_free(body);
  }

  arena_free(local);
  return ret;
}
