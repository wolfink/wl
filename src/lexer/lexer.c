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

int lexer_handle_keywords(Arena* context, Lexer* lex, string* in, int index)
{
  switch(u_getc(in, index))
  {
  case 'b':
    if (u_strcmp(u_strslice(context, in, index, index + 5), u_strnew(context, "break")) == 0) {
      lexer_add_token(context, lex, TokenType_BREAK, NULL);
      return 1;
    }
  case 'd':
    if (u_getc(in, index + 1) == 'o') {
      lexer_add_token(context, lex, TokenType_DO, NULL);
      return 1;
    }
    break;
  case 'e':
    switch(u_getc(in, index + 1))
    {
    case 'l':
      if (u_strcmp(u_strslice(context, in, index, index + 4), u_strnew(context, "else")) == 0) {
        lexer_add_token(context, lex, TokenType_ELSE, NULL);
        return 1;
      }
    case 'n':
      if (u_strcmp(u_strslice(context, in, index, index + 3), u_strnew(context, "env")) == 0) {
        lexer_add_token(context, lex, TokenType_ENV, NULL);
        return 1;
      }
    }
    break;
  case 'f':
    if (u_strcmp(u_strslice(context, in, index, index + 3), u_strnew(context, "for")) == 0) {
      lexer_add_token(context, lex, TokenType_FOR, NULL);
      return 1;
    }
    break;
  case 'i':
    switch (u_getc(in, index + 1)) {
    case 'f':
      lexer_add_token(context, lex, TokenType_IF, NULL);
      return 1;
    case 'n':
      if (u_strcmp(u_strslice(context, in, index, index + 6), u_strnew(context, "inline")) == 0) {
        lexer_add_token(context, lex, TokenType_INLINE, NULL);
        return 1;
      }
    }
    break;
  case 'm':
    if (u_strcmp(u_strslice(context, in, index, index + 3), u_strnew(context, "mut")) == 0) {
      lexer_add_token(context, lex, TokenType_MUT, NULL);
      return 1;
    }
    break;
  case 's':
    if (u_strcmp(u_strslice(context, in, index, index + 6), u_strnew(context, "switch")) == 0) {
      lexer_add_token(context, lex, TokenType_SWITCH, NULL);
      return 1;
    }
    break;
  case 'w':
    if (u_strcmp(u_strslice(context, in, index, index + 5), u_strnew(context, "while")) == 0) {
      lexer_add_token(context, lex, TokenType_WHILE, NULL);
      return 1;
    }
    break;
  }
  return 0;
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

#define X(name, second, str)\
    case second:\
      lexer_add_token(context, lex, TokenType_##name, NULL);\
      index++;\
      break;\

#define X_OVERLOAD(first, TokenTypeTable, base)\
    case first:\
      switch(u_getc(in, index + 1))\
      {\
        TokenTypeTable\
        default:\
          lexer_add_token(context, lex, TokenType_##base, NULL);\
      }\
      break;
    
    X_OVERLOAD('&', TokenTypeTableAnd, BW_AND);
    X_OVERLOAD('=', TokenTypeTableAssign, ASSIGN);
    X_OVERLOAD('<', TokenTypeTableAngle, LANGLE);
    X_OVERLOAD(':', TokenTypeTableColon, COLON);
    X_OVERLOAD('-', TokenTypeTableMinus, MINUS);
    X_OVERLOAD('|', TokenTypeTableOr, BW_OR);

#undef X
#undef X_OVERLOAD

    case '0':
      switch(u_getc(in, index + 1))
      {
      case 'x': 
        index = lexer_add_string_token(context, lex, TokenType_HEX, is_xdigit, in, index + 2);
        break;
      case'0': case'1': case'2': case'3':
      case'4': case'5': case'6': case'7':
        index = lexer_add_string_token(context, lex, TokenType_OCTAL, is_octal, in, index + 1);
      case'b':
        index = lexer_add_string_token(context, lex, TokenType_BINARY, is_binary, in, index + 2);
      }
      break;

    case'a':case'b':case'c':case'd':case'e':case'f':case'g':case'h':case'i':case'j':case'k':case'l':case'm':
    case'n':case'o':case'p':case'q':case'r':case's':case't':case'u':case'v':case'w':case'x':case'y':case'z':
      if (lexer_handle_keywords(b, lex, in, index)) break;

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
