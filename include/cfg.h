#ifndef CFG_H
#define CFG_H

#include <parser.h>

typedef struct {} ControlFlowGraph;

extern ControlFlowGraph* cfg_create(Arena*);
extern void              cfg_scan_ast(ControlFlowGraph*, AST* in);
extern AST*              cfg_get_stmt(ControlFlowGraph*, size_t node_no, size_t index);
extern string*           cfg_to_string(Arena* context, ControlFlowGraph*);

#endif
