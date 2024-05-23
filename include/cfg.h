#ifndef CFG_H
#define CFG_H

#include <parser.h>

typedef struct{
  const vector_object stmts;
  string* const environment;
  const size_t scope_no;
  int generated;
}ControlFlowGraphNode;

typedef struct {} ControlFlowGraph;

extern ControlFlowGraph*     cfg_create(Arena*);
extern void                  cfg_scan_ast(ControlFlowGraph*, AST* in);
extern ControlFlowGraphNode* cfg_get_node(ControlFlowGraph*, size_t node_no);
size_t                       cfg_get_len(ControlFlowGraph*);
extern AST*                  cfg_get_stmt(ControlFlowGraph*, size_t node_no, size_t index);
extern string*               cfg_to_string(Arena* context, ControlFlowGraph*);

#endif
