#ifndef GEN_H
#define GEN_H


#ifndef GEN_INTERNAL_H

typedef void Parser;
typedef void Arena;
typedef void string;

#endif

extern string* gen_code_from_parser(Arena*, Parser*);

#endif
