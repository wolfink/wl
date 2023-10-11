#include "../lexer.h"
#include "../defs.h"
#include <string.h>
#include <ctype.h>

#define MAX_TOKEN_LEN KB(1) - 1
#define LEXER_DEFAULT_SIZE KB(80)

#define TokenTypeTableSimple \
  X(SEMI,         ';', ";") \
  X(LBRACE,       '{', "{")\
  X(RBRACE,       '}', "}")\
  X(LPAR,         '(', "(")\
  X(RPAR,         ')', ")")\
  X(PERIOD,       '.', ".")\
  X(STAR,         '*', "*")\
  X(PLUS,         '+', "+")\
  X(CARAT,        '^', "^")\
  X(AND,          '&', "&")\
  X(COMMA,        ',', ",")\
  X(TYPE_ASSIGN,  ':', ":")\

#define TokenTypeTableOverload \
  X(MINUS,        '-', "-")\
  X(SMALLARROW,   '-', "->")\
  X(ASSIGN,       '=', "=")\
  X(BIGARROW,     '=', "=>")\
  X(EQUALS,       '=', "==")\

#define TokenTypeTableStmts \
  X(NUMBER,      '\0', "num")\
  X(ID,          '\0', "id")\

#define TokenTypeTable \
  TokenTypeTableSimple\
  TokenTypeTableOverload\
  TokenTypeTableStmts

typedef enum {
#define X(name, first, str) TokenType_##name,
  TokenTypeTable
#undef X
  TokenType_COUNT
} TokenType;

char token_type_first[TokenType_COUNT] = {
#define X(name, first, str) first,
  TokenTypeTable
#undef X
};

char *token_type_str[TokenType_COUNT] = {
#define X(name, first, str) str,
  TokenTypeTable
#undef X
};

typedef struct
{
  size_t size;
  size_t len;
  size_t strlen;
  string** tokens; // TODO: instead of direct pointers, store offsets from base of Arena
} Lexer;

Lexer* lexer_create(Arena* a)
{
  Lexer* ret = arena_alloc(a, sizeof(Lexer));
  ret->tokens = arena_alloc(a, sizeof(string*) * LEXER_DEFAULT_SIZE);
  ret->size = LEXER_DEFAULT_SIZE;
  ret->len = 0;
  ret->strlen = 0;
  return ret;
}

// TODO: fix function
int lexer_resize(Arena* a, Lexer* lex, size_t new_size)
{
  string** new_tokens = arena_alloc(a, sizeof(string*) * new_size);
  if (!new_tokens) return 1;

  memcpy(new_tokens, lex->tokens, lex->size);
  lex->tokens = new_tokens;
  return 0;
}

string* lexer_next_token(string* in)
{
  return NULL;
}

int lexer_add_token(
    Arena* context,
    Lexer* lex,
    TokenType t,
    const string* data)
{
  Arena* a = arena_create();
  if (!a) return 1;

  string* worker;
  char newstr[4];
  int pos = 0;

  worker = u_strnew(a, "(");
  worker = u_strcat(a, worker, u_strnew(a, token_type_str[t]));
  if (   t == TokenType_ID
      || t == TokenType_NUMBER)
    worker = u_strcat(a, worker, u_strnew(a, ","));
  worker = u_strcat(a,
      (data) ? u_strcat(a, worker, data): worker,
      u_strnew(a, ")"));

  if (lex->len + 1 > lex->size) lexer_resize(context, lex, lex->size * 2);
  worker = u_strcpyar(context, worker);
  lex->tokens[lex->len++] = worker;
  lex->strlen += u_strlen(worker);

  arena_free(a);
  return 0;
}

string* lexer_analyze(Arena *context, string* in)
{
  Arena *a = arena_create_init(MB(1));
  if(!a) return NULL;
  Lexer* lex = lexer_create(a);
  if (u_strlen(in) == 0) return NULL;

  for(int index = 0; index < u_strlen(in); index++)
  {
    Arena *b = arena_create_init(MAX_TOKEN_LEN + sizeof(size_t) + 1);

    char c = u_getc(in, index);
    char temp[MAX_TOKEN_LEN + 1];

    switch(c)
    {
#define X(name, first, str) \
      case  first:\
        lexer_add_token(a, lex, TokenType_##name, NULL);\
        break;
      TokenTypeTableSimple
#undef X
      case '-':
        if(u_getc(in, index + 1) == '>') {
          lexer_add_token(a, lex, TokenType_SMALLARROW, NULL);
          index++;
        }
        else
          lexer_add_token(a, lex, TokenType_MINUS, NULL);
        break;
      case '=':
        switch(u_getc(in, index + 1))
        {
          case '=':
            lexer_add_token(a, lex, TokenType_EQUALS, NULL);
            index++;
            break;
          case '>':
            lexer_add_token(a, lex, TokenType_BIGARROW, NULL);
            index++;
            break;
          default:
            lexer_add_token(a, lex, TokenType_ASSIGN, NULL);
        }
        break;
      default:
        if (isdigit(c)) {
          size_t pos = 0;
          while (pos < MAX_TOKEN_LEN && index < u_strlen(in) && isdigit(c)) {
            temp[pos++] = c;
            c = u_getc(in, ++index);
          }
          --index;
          temp[pos] = '\0';
          string* number = u_strnew(b, temp);
          lexer_add_token(a, lex, TokenType_NUMBER, number);
        } else if (isalpha(c)) {
          size_t pos = 0;
          while(pos < MAX_TOKEN_LEN && index < u_strlen(in) && isalpha(c)) {
            temp[pos++] = c;
            c = u_getc(in, ++index);
          }
          --index;
          temp[pos] = '\0';
          string* id = u_strnew(b, temp);
          lexer_add_token(a, lex, TokenType_ID, id);
        }
        break;
    };

    arena_free(b);
  }

  char *builder = arena_alloc(a, lex->strlen + 1);
  size_t pos = 0;
  for(size_t i = 0; i < lex->len; i++)
  {
    string* token = lex->tokens[i];
    for(int j = 0; j < u_strlen(token); j++)
    {
      builder[pos++] = u_getc(token, j);
    }
  }
  builder[pos] = '\0';
  string* ret = u_strnew(context, builder);

  arena_free(a);
  return ret;
}
