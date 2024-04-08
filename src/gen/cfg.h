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
void cfg_scan_ast(ControlFlowGraph* c, AST* in);
string* cfg_to_string(ControlFlowGraph*);
void cfg_add_node(ControlFlowGraph*, ControlFlowGraphNode*);

ControlFlowGraphNode* cfgn_create(ControlFlowGraph*);
void cfgn_add_connection(ControlFlowGraphNode* n, size_t index);
void cfgn_populate(ControlFlowGraphNode*, AST*);

#endif
