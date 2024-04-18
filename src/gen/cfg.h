#ifndef CFG_H
#define CFG_H

#include <parser.h>

struct CFG;

typedef struct {
  struct CFG* parent;
  size_t stmt_len;
  size_t stmt_size;
  AST**  stmts;
  size_t conn_len;
  size_t conn_size;
  size_t* connection_list;
} ControlFlowGraphNode;

typedef struct CFG {
  Arena* memory;
  ControlFlowGraphNode** node_list;
  size_t size;
  size_t len;
} ControlFlowGraph;

ControlFlowGraph* cfg_create(Arena*);
void cfg_scan_ast(ControlFlowGraph*, AST* in);
void cfg_populate_node(ControlFlowGraph*, ControlFlowGraphNode*, AST*);
size_t cfg_handle_branch(ControlFlowGraph*, AST* branch_stmt);
size_t cfg_handle_do(ControlFlowGraph*, AST* do_stmt);
size_t cfg_handle_while(ControlFlowGraph*, AST* while_stmt);
string* cfg_to_string(Arena* context, ControlFlowGraph*, Lexer*);
void cfg_add_node(ControlFlowGraph*, ControlFlowGraphNode*);

ControlFlowGraphNode* cfgn_create(ControlFlowGraph*);
void cfgn_add_connection(ControlFlowGraphNode* n, size_t index);
void cfgn_add_stmt(ControlFlowGraphNode* n, AST* stmt);
string* cfgn_to_string(Arena* context, ControlFlowGraphNode*, Lexer*, size_t index);

#endif
