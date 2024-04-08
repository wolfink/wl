#include"../test.h"

typedef struct {
  int len;
  char* lines[100];
} ParserOutput;

int test_command_ids[] = {
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12
};

ParserOutput test_commands_expected[NUM_COMMANDS] = {
  {
    12,
    {
      "PROGRAM\n",
      "| STMT_LIST\n",
      "| | STMT\n",
      "| | | EXPR\n",
      "| | | | ASSIGN\n",
      "| | | | VAR\n",
      "| | | | | ID: x\n",
      "| | | | | TYPE\n",
      "| | | | | | ID: int\n",
      "| | | | EXPR\n",
      "| | | | | LITERAL\n",
      "| | | | | | NUMBER: 1000\n"
    }
  },
  {
    14,
    {
      "PROGRAM\n",
      "| STMT_LIST\n",
      "| | STMT\n",
      "| | | EXPR\n",
      "| | | | ASSIGN\n",
      "| | | | VAR\n",
      "| | | | | ID: y\n",
      "| | | | | TYPE\n",
      "| | | | | | ID: float\n",
      "| | | | EXPR\n",
      "| | | | | LITERAL\n",
      "| | | | | | FLOAT\n",
      "| | | | | | | NUMBER: 200\n",
      "| | | | | | | NUMBER: 1\n"
    }
  },
  {
    26,
    {
      "PROGRAM\n",
      "| STMT_LIST\n",
      "| | STMT\n",
      "| | | EXPR\n",
      "| | | | VAR\n",
      "| | | | | ID: z\n",
      "| | | | | TYPE\n",
      "| | | | | | TUPLE_TYPE\n",
      "| | | | | | | TUPLE_TYPE_BODY\n",
      "| | | | | | | | TTB_TYPE\n",
      "| | | | | | | | | VAR\n",
      "| | | | | | | | | | ID: x\n",
      "| | | | | | | | | | TYPE\n",
      "| | | | | | | | | | | ID: int\n",
      "| | | | | | | | TTB_TYPE\n",
      "| | | | | | | | | VAR\n",
      "| | | | | | | | | | ID: y\n",
      "| | | | | | | | | | TYPE\n",
      "| | | | | | | | | | | ID: float\n",
      "| | | | | | TUPLE_TYPE\n",
      "| | | | | | | TUPLE_TYPE_BODY\n",
      "| | | | | | | | TTB_TYPE\n",
      "| | | | | | | | | VAR\n",
      "| | | | | | | | | | ID: ret\n",
      "| | | | | | | | | | TYPE\n",
      "| | | | | | | | | | | ID: int\n"
    }
  },
  {
    49,
    {
      "PROGRAM\n",
      "| STMT_LIST\n",
      "| | STMT\n",
      "| | | BRANCH\n",
      "| | | | EXPR\n",
      "| | | | | EXPR_AND\n",
      "| | | | | | EXPR_REL\n",
      "| | | | | | | VAR\n",
      "| | | | | | | | ID: x\n",
      "| | | | | | | RANGLE\n",
      "| | | | | | | LITERAL\n",
      "| | | | | | | | NUMBER: 3\n",
      "| | | | | | AND\n",
      "| | | | | | EXPR_REL\n",
      "| | | | | | | VAR\n",
      "| | | | | | | | ID: y\n",
      "| | | | | | | LANGLE\n",
      "| | | | | | | LITERAL\n",
      "| | | | | | | | NUMBER: 2\n",
      "| | | | BLOCK\n",
      "| | | | | STMT_LIST\n",
      "| | | | | | STMT\n",
      "| | | | | | | EXPR\n",
      "| | | | | | | | ASSIGN\n",
      "| | | | | | | | VAR\n",
      "| | | | | | | | | ID: x\n",
      "| | | | | | | | EXPR\n",
      "| | | | | | | | | CALL\n",
      "| | | | | | | | | | VAR\n",
      "| | | | | | | | | | | ID: z\n",
      "| | | | | | | | | | TUPLE\n",
      "| | | | | | | | | | | TUPLE_BODY\n",
      "| | | | | | | | | | | | EXPR\n",
      "| | | | | | | | | | | | | VAR\n",
      "| | | | | | | | | | | | | | ID: x\n",
      "| | | | | | | | | | | | EXPR\n",
      "| | | | | | | | | | | | | VAR\n",
      "| | | | | | | | | | | | | | ID: y\n",
      "| | | | ELSE\n",
      "| | | | | BLOCK\n",
      "| | | | | | STMT_LIST\n",
      "| | | | | | | STMT\n",
      "| | | | | | | | EXPR\n",
      "| | | | | | | | | ASSIGN\n",
      "| | | | | | | | | VAR\n",
      "| | | | | | | | | | ID: x\n",
      "| | | | | | | | | EXPR\n",
      "| | | | | | | | | | LITERAL\n",
      "| | | | | | | | | | | NUMBER: 1\n",
    }
  },
  {
    21,
    {
      "PROGRAM\n",
      "| STMT_LIST\n",
      "| | STMT\n",
      "| | | EXPR\n",
      "| | | | ASSIGN\n",
      "| | | | VAR\n",
      "| | | | | ID: x\n",
      "| | | | EXPR\n",
      "| | | | | EXPR_SUM\n",
      "| | | | | | EXPR_MUL\n",
      "| | | | | | | VAR\n",
      "| | | | | | | | ID: x\n",
      "| | | | | | | STAR\n",
      "| | | | | | | LITERAL\n",
      "| | | | | | | | NUMBER: 2\n",
      "| | | | | | PLUS\n",
      "| | | | | | LITERAL\n",
      "| | | | | | | NUMBER: 1000\n",
      "| | | | | | MINUS\n",
      "| | | | | | LITERAL\n",
      "| | | | | | | NUMBER: 30\n",
    }
  },
  {
    10,
    {
      "PROGRAM\n",
      "| STMT_LIST\n",
      "| | STMT\n",
      "| | | EXPR\n",
      "| | | | COLON-ASSIGN\n",
      "| | | | VAR\n",
      "| | | | | ID: w\n",
      "| | | | EXPR\n",
      "| | | | | LITERAL\n",
      "| | | | | | HEX: 1adf\n",
    }
  },
  {
    10,
    {
      "PROGRAM\n",
      "| STMT_LIST\n",
      "| | STMT\n",
      "| | | EXPR\n",
      "| | | | COLON-ASSIGN\n",
      "| | | | VAR\n",
      "| | | | | ID: a\n",
      "| | | | EXPR\n",
      "| | | | | LITERAL\n",
      "| | | | | | BINARY: 0011\n",
    }
  },
  {
    10,
    {
      "PROGRAM\n",
      "| STMT_LIST\n",
      "| | STMT\n",
      "| | | EXPR\n",
      "| | | | COLON-ASSIGN\n",
      "| | | | VAR\n",
      "| | | | | ID: b\n",
      "| | | | EXPR\n",
      "| | | | | LITERAL\n",
      "| | | | | | OCTAL: 200\n",
    }
  },
  {
    11,
    {
      "PROGRAM\n",
      "| STMT_LIST\n",
      "| | STMT\n",
      "| | | EXPR\n",
      "| | | | COLON-ASSIGN\n",
      "| | | | VAR\n",
      "| | | | | ID: c\n",
      "| | | | EXPR\n",
      "| | | | | LITERAL\n",
      "| | | | | | FLOAT\n",
      "| | | | | | | NUMBER: 300\n",
    }
  },
  {
    11,
    {
      "PROGRAM\n",
      "| STMT_LIST\n",
      "| | STMT\n",
      "| | | EXPR\n",
      "| | | | COLON-ASSIGN\n",
      "| | | | VAR\n",
      "| | | | | ID: d\n",
      "| | | | EXPR\n",
      "| | | | | LITERAL\n",
      "| | | | | | FLOAT\n",
      "| | | | | | | NUMBER: 400\n",
    }
  },
  {
    29,
    {
      "PROGRAM\n",
      "| STMT_LIST\n",
      "| | STMT\n",
      "| | | EXPR\n",
      "| | | | ASSIGN\n",
      "| | | | VAR\n",
      "| | | | | ID: e\n",
      "| | | | | TYPE\n",
      "| | | | | | TUPLE_TYPE\n",
      "| | | | | | | TUPLE_TYPE_BODY\n",
      "| | | | | | | | TTB_TYPE\n",
      "| | | | | | | | | VAR\n",
      "| | | | | | | | | | ID: a\n",
      "| | | | | | | | | | TYPE\n",
      "| | | | | | | | | | | ID: int\n",
      "| | | | | | | | TTB_TYPE\n",
      "| | | | | | | | | VAR\n",
      "| | | | | | | | | | ID: b\n",
      "| | | | | | | | | | TYPE\n",
      "| | | | | | | | | | | ID: int\n",
      "| | | | EXPR\n",
      "| | | | | TUPLE\n",
      "| | | | | | TUPLE_BODY\n",
      "| | | | | | | EXPR\n",
      "| | | | | | | | LITERAL\n",
      "| | | | | | | | | NUMBER: 1\n",
      "| | | | | | | EXPR\n",
      "| | | | | | | | LITERAL\n",
      "| | | | | | | | | NUMBER: 2\n",
    }
  },
  {
    12,
    {
      "PROGRAM\n",
      "| STMT_LIST\n",
      "| | STMT\n",
      "| | | EXPR\n",
      "| | | | ASSIGN\n",
      "| | | | VAR\n",
      "| | | | | ID: f\n",
      "| | | | EXPR\n",
      "| | | | | VAR\n",
      "| | | | | | ID: e\n",
      "| | | | | | VAR\n",
      "| | | | | | | ID: a\n",
    }
  }
};
