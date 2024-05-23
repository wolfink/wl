#ifndef GEN_H
#define GEN_H

#include <util/arena.h>
#include <parser.h>
#include <cfg.h>

typedef struct {} Generator;

extern Generator* generator_create(Arena*, const char* filename, ControlFlowGraph*);
extern void       generate(Generator*);

#endif
