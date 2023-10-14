#include "lexer_internal.h"
#include "../lexer.h"
#include "../defs.h"
#include <ctype.h>
#include <stdio.h>
#include <wctype.h>

Lexer* lexer_new(Arena* a)
{
  Lexer* ret = arena_alloc(a, sizeof(Lexer));
  ret->tokens = arena_alloc(a, sizeof(TokenType) * LEXER_DEFAULT_SIZE);
  ret->token_strings = arena_alloc(a, sizeof(string) * LEXER_DEFAULT_SIZE);
  ret->size = LEXER_DEFAULT_SIZE;
  ret->len = 0;
  ret->strlen = 0;
  return ret;
}

// TODO: fix function
int lexer_resize(Arena* a, Lexer* lex, size_t new_size)
{
  string** new_token_strings = arena_alloc(a, sizeof(string*) * new_size);
  TokenType* new_tokens = arena_alloc(a, sizeof(TokenType) * new_size);
  if (!new_token_strings || !new_tokens) return 1;

  memcpy(new_token_strings, lex->token_strings, lex->size);
  memcpy(new_tokens, lex->tokens, lex->size);
  lex->token_strings = new_token_strings;
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
  int pos = 0;

  worker = u_strnew(a, "(");
  worker = u_strcat(a, worker, u_strnew(a, token_type_str[t]));
  if (   t == TokenType_ID
      || t == TokenType_NUMBER)
    worker = u_strcat(a, worker, u_strnew(a, ","));
  worker = u_strcat(a,
      (data) ? u_strcat(a, worker, data): worker,
      u_strnew(a, ")"));

  if (lex->len > lex->size) lexer_resize(context, lex, lex->size * 2);
  worker = u_strcpyar(context, worker);
  lex->token_strings[lex->len] = worker;
  lex->tokens[lex->len++] = t;
  lex->strlen += u_strlen(worker);

  arena_free(a);
  return 0;
}

inline int lexer_add_string_token(Arena*, Lexer*, TokenType, CTypeFunction matcher, string* in, size_t start);

int lexer_add_string_token(
    Arena* context,
    Lexer* lex,
    TokenType t,
    CTypeFunction matcher,
    string* in,
    size_t start)
{
    Arena* lcl = arena_create();
    char temp[MAX_TOKEN_LEN];
    size_t index = start;
    size_t pos = 0;
    char c = u_getc(in, index);
    while(pos < MAX_TOKEN_LEN
       && index < u_strlen(in)
       && matcher(c)) {
      temp[pos++] = c;
      c = u_getc(in, ++index);
    }
    temp[pos] = '\0';
    lexer_add_token(context, lex, t, u_strnew(lcl, temp));
    arena_free(lcl);
    return index - 1;
}

Lexer* lexer_create(Arena *context, string* in)
{
  Arena *a = arena_create();
  if(!a) return NULL;
  Lexer* lex = lexer_new(context);
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
        lexer_add_token(context, lex, TokenType_##name, NULL);\
        break;
      TokenTypeTableSimple
#undef X
      case '-':
        if(u_getc(in, index + 1) == '>') {
          lexer_add_token(context, lex, TokenType_SMALLARROW, NULL);
          index++;
        }
        else
          lexer_add_token(context, lex, TokenType_MINUS, NULL);
        break;
      case '=':
        switch(u_getc(in, index + 1))
        {
          case '=':
            lexer_add_token(context, lex, TokenType_EQUALS, NULL);
            index++;
            break;
          case '>':
            lexer_add_token(context, lex, TokenType_BIGARROW, NULL);
            index++;
            break;
          default:
            lexer_add_token(context, lex, TokenType_ASSIGN, NULL);
        }
        break;
      case '0':
        switch(u_getc(in, index + 1))
        {
          case 'x': 
            index = lexer_add_string_token(context, lex, TokenType_HEX, is_xdigit, in, index);
            break;
          case'0': case'1': case'2': case'3':
          case'4': case'5': case'6': case'7':
            index = lexer_add_string_token(context, lex, TokenType_OCTAL, is_octal, in, index);
          case'b':
            index = lexer_add_string_token(context, lex, TokenType_BINARY, is_binary, in, index);
        }
      default:
        if (isdigit(c)) {
          index = lexer_add_string_token(context, lex, TokenType_NUMBER, is_digit, in, index);
        } else if (isalpha(c)) {
          index = lexer_add_string_token(context, lex, TokenType_ID, is_alnum, in, index);
        }
        break;
    };

    arena_free(b);
  }

  arena_free(a);
  return lex;
}

string* lexer_get_value_at_index(Arena* context, Lexer* lex, size_t index)
{
  TokenType t = lex->tokens[index];
  switch(t)
  {
    case TokenType_ID: case TokenType_NUMBER:
      return u_strslice(context, lex->token_strings[index], strlen(token_type_str[t]) + 2, -1);
    default:
      return NULL;
  }
}

TokenType lexer_get_token_type_at_index(Lexer* lex, size_t index)
{
  return lex->tokens[index];
}

size_t lexer_get_len(Lexer* lex)
{
  return lex->len;
}

string* lexer_to_string(Arena* context, Lexer* lex)
{
  Arena* a = arena_create();
  char *builder = arena_alloc(a, lex->strlen + 1);
  size_t pos = 0;
  for(size_t i = 0; i < lex->len; i++)
  {
    string* token = lex->token_strings[i];
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
