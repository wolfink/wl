#include <util.h>
#include <parser.h>

typedef struct {} ControlFlowGraph;

extern string* gen_code_from_parser(Arena*, Parser*);
string* cfg_to_string(Arena* context, ControlFlowGraph*);
ControlFlowGraph* cfg_create(Arena*);
void cfg_scan_ast(ControlFlowGraph*, AST* in);
