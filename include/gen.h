#ifndef GEN_H
#define GEN_H

#include <util.h>
#include <parser.h>

typedef struct {} Generator;

Generator* generator_create(Arena*, const char* filename);
void       generate(Generator*);

#endif
