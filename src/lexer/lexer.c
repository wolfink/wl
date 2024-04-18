#include "lexer_internal.h"
#include "../include/lexer.h"
#include "../include/defs.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>

string* token_type_tostr(Arena* a, TokenType t)
{
  switch(t)
  {
#define X(name, first, str)\
  case TokenType_##name:\
    return u_strnew(a, str);
  TokenTypeTable
#undef X
  default:
    return u_strnew(a, "???");
  }
}

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
  if (data != NULL)
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
    Arena* local = arena_create();
    char temp[MAX_TOKEN_LEN];
    size_t index = start;
    size_t pos = 0;
    char c = u_getc(in, index);
    while(pos < MAX_TOKEN_LEN
       && index < u_strlen(in)
       && matcher(c)) {
      temp[pos++] = c;
      c = u_getc(in, ++index);
      // Handle float period
    }
    temp[pos] = '\0';
    lexer_add_token(context, lex, t, u_strnew(local, temp));
    arena_free(local);
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

    uint64_t* str_int = (u_int64_t*) u_strslice(b, in, index, index + 8);
    char temp[MAX_TOKEN_LEN + 1];

#define X(name, number, str)\
  case number:\
    lexer_add_token(context, lex, TokenType_##name, NULL);\
    index += TOKEN_SIZE - 1;\
    continue;
#define T(a) ((u_int64_t) a)

    // Handle tokens with 6 characters
#define TOKEN_SIZE 6
    switch(*str_int & (T(-1) >> 8 * 2))
    {
      TokenTypeTable6
    }
#undef TOKEN_SIZE

    // Handle tokens with 5 characters
#define TOKEN_SIZE 5
    switch(*str_int & (T(-1) >> 8 * 3))
    {
      TokenTypeTable5
    }
#undef TOKEN_SIZE

    // Handle tokens with 4 characters
#define TOKEN_SIZE 4
    switch(*str_int & (T(-1) >> 8 * 4))
    {
      TokenTypeTable4
    }
#undef TOKEN_SIZE

    // Handle tokens with 3 characters
#define TOKEN_SIZE 3
    switch(*str_int & (T(-1) >> 8 * 5))
    {
      TokenTypeTable3
    }
#undef TOKEN_SIZE

    // Handle tokens with 2 characters
#define TOKEN_SIZE 2
    switch(*str_int & (T(-1) >> 8 * 6))
    {
      TokenTypeTable2
    }
#undef TOKEN_SIZE

    // Handle tokens with 1 character and string tokens
#define TOKEN_SIZE 1
    char c = *str_int & 255;
    switch(c)
    {
      TokenTypeTable1

      case '0':
        switch(u_getc(in, index + 1))
        {
        case 'x': 
          index = lexer_add_string_token(context, lex, TokenType_HEX, is_xdigit, in, index + 2);
          break;
        case'0': case'1': case'2': case'3':
        case'4': case'5': case'6': case'7':
          index = lexer_add_string_token(context, lex, TokenType_OCTAL, is_octal, in, index + 1);
          break;
        case'b':
          index = lexer_add_string_token(context, lex, TokenType_BINARY, is_binary, in, index + 2);
          break;
        case'.':
          lexer_add_token(context, lex, TokenType_PERIOD, NULL);
          index++;
          break;
        default:
          index = lexer_add_string_token(context, lex, TokenType_NUMBER, is_digit, in, index);
        }
        break;

      default:
        if (isdigit(c)) {
          TokenType t = TokenType_NUMBER;

          for (int i = 0; i < strlen(in) && isdigit(c = u_getc(in, index + i)); i++) {}
          if (!isspace(c) && c != 'f') {
            //TODO: raise error invalid suffix 
          }
          index = lexer_add_string_token(context, lex, TokenType_NUMBER, is_digit, in, index);
        } else if (isalpha(c)) {
          index = lexer_add_string_token(context, lex, TokenType_ID, is_alnum, in, index);
        }
    }
#undef X
#undef TOKEN_SIZE
#undef T

    arena_free(b);
  }

  arena_free(a);
  return lex;
}

string* lexer_get_value_at_index(Arena* context, Lexer* lex, size_t index)
{
  if (index > lex->strlen) {
    printf("lexer_get_value_at_index(context = %p, lex = %p, index = %lu): index value too large\n", context, context, index);
    return u_strnew(context, "");
  }
  TokenType t = lex->tokens[index];
  switch(t)
  {
    case TokenType_ID: case TokenType_NUMBER: case TokenType_HEX: case TokenType_OCTAL: case TokenType_BINARY:
      return u_strslice(context, lex->token_strings[index], strlen(token_type_str[t]) + 2, u_strlen(lex->token_strings[index]));
    default:
      return NULL;
  }
}

TokenType lexer_get_token_type_at_index(Lexer* lex, size_t index)
{
  if (index > lex->len) {
    die("error: lexer_get_token_type_at_index(lex = %p, index = %lu): index out of range\n", lex, index);
  }
  if (index == lex->len) return TokenType_EOF;
  return lex->tokens[index];
}

size_t lexer_get_len(Lexer* lex)
{
  return lex->len;
}

string* lexer_to_string(Arena* context, Lexer* lex)
{
  Arena* a = arena_create();
  char *builder = arena_alloc(a, lex->strlen + 6);
  size_t pos = 0;
  for(size_t i = 0; i < lex->len; i++)
  {
    string* token = lex->token_strings[i];
    for(int j = 0; j < u_strlen(token); j++)
    {
      builder[pos++] = u_getc(token, j);
    }
  }
  strncpy(builder + pos, "(EOF)", 6);
  string* ret = u_strnew(context, builder);

  arena_free(a);
  return ret;
}
