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

vector_impl(Token, 100)
vector_impl(object, 100)

// TODO: fix function
// int lexer_resize(Arena* a, Lexer* lex, size_t new_size)
// {
//   string** new_token_strings = arena_alloc(a, sizeof(string*) * new_size);
//   TokenType* new_tokens = arena_alloc(a, sizeof(TokenType) * new_size);
//   if (!new_token_strings || !new_tokens) return 1;

//   memcpy(new_token_strings, lex->token_strings, lex->size);
//   memcpy(new_tokens, lex->tokens, lex->size);
//   lex->token_strings = new_token_strings;
//   lex->tokens = new_tokens;
//   return 0;
// }

string* lexer_next_token(string* in)
{
  return NULL;
}

int lexer_add_token(
    Arena* context,
    Lexer* lex,
    const Token* token)
{
  Arena* a = arena_create();
  Line* curr_line = lex->lines->values[lex->curr_line];

  if (!a) return 1;

  if (curr_line == NULL) {
    curr_line = vector_Token_create(lex->lines->mem);
    vector_object_add(lex->lines, curr_line);
  }
  vector_Token_add(curr_line, *token);

  arena_free(a);
  return 0;
}

// inline int lexer_add_string_token(Arena*, Lexer*, TokenType, CTypeFunction matcher, string* in, size_t start);

int lexer_add_string_token(
    Arena* context,
    Lexer* lex,
    TokenType t_type,
    CTypeFunction matcher,
    string* in,
    size_t start,
    size_t line_pos)
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
    Token new_token = { t_type, u_strnew(local, temp), line_pos, line_pos + index - start };
    lexer_add_token(context, lex, &new_token);
    arena_free(local);
    return index - 1;
}

Lexer* lexer_create(Arena* a, const char* filename)
{
  Lexer* ret = arena_alloc(a, sizeof(Lexer));
  ret->mem = a;
  ret->filename = filename;
  ret->curr_line = 0;
  ret->lines = vector_object_create(a);
  return ret;
}

// Output: int => 0 on success, 1 on failure
int lexer_scan(Lexer* lex, string* in)
{
  Arena* context = lex->mem;
  Arena* a = arena_create();
  if(!a) return 1;
  // vector_object_add(lex->lines, vector_Token_create(context));
  if (u_strlen(in) == 0) return 1;

  for(int index = 0, line_pos = 0; index < u_strlen(in); index++, line_pos++)
  {
    Arena *b = arena_create_init(MAX_TOKEN_LEN + sizeof(size_t) + 1);

    uint64_t* str_int = (u_int64_t*) u_strslice(b, in, index, index + 8);
    char temp[MAX_TOKEN_LEN + 1];

#define X(name, number, str)\
  case number:{\
    const size_t advance = TOKEN_SIZE - 1;\
    Token new_token = { TokenType_##name, NULL, line_pos, line_pos + advance };\
    lexer_add_token(context, lex, &new_token);\
    index += advance;\
    continue;\
  }
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
    case CHAR_SUM_2('/', '/'):
      index += 2;
      while(index < u_strlen(in) && in[++index] != '\n'); // Move index past the end of the line
      lex->curr_line++;
      vector_object_add(lex->lines, vector_Token_create(context));
      line_pos = 0;
      continue;
    case CHAR_SUM_2('/', '*'):
      index += 2;
      // Move index past "*/" string
      while(index < u_strlen(in) - 1
            && !(in[++index] == '*' && in[index] == '/')) {
        if (in[index] == '\n') {
          lex->curr_line++;
          vector_object_add(lex->lines, vector_Token_create(context));
          line_pos = 0;
        }
      }; 
      continue;
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
          index = lexer_add_string_token(context, lex, TokenType_HEX, is_xdigit, in, index + 2, line_pos);
          break;
        case'0': case'1': case'2': case'3':
        case'4': case'5': case'6': case'7':
          index = lexer_add_string_token(context, lex, TokenType_OCTAL, is_octal, in, index + 1, line_pos);
          break;
        case'b':
          index = lexer_add_string_token(context, lex, TokenType_BINARY, is_binary, in, index + 2, line_pos);
          break;
        case'.':
          if (index + 2 < u_strlen(in) && u_getc(in, index + 2) == '.');
          else {
            Token new_token = { TokenType_PERIOD, NULL, line_pos, line_pos + 1 };
            lexer_add_token(context, lex, &new_token);
            index++;
            break;
          }
        default:
          index = lexer_add_string_token(context, lex, TokenType_NUMBER, is_digit, in, index, line_pos);
        }
        break;
      case'\"':
        index = lexer_add_string_token(context, lex, TokenType_STRING, isn_dquote, in, index + 1, line_pos) + 1;
        break;
      case'\'':
        index = lexer_add_string_token(context, lex, TokenType_QUOTE, isn_quote, in, index + 1, line_pos) + 1;
        break;
      case'#':
        if (u_getc(in, index + 1) == '(') {
          index = lexer_add_string_token(context, lex, TokenType_MACRO, isn_rpar, in, index, line_pos) + 1;
        } else if (u_getc(in, index + 1) == '{'){
          index = lexer_add_string_token(context, lex, TokenType_MACRO, isn_rbrace, in, index + 1, line_pos) + 1;
        } else {
          index = lexer_add_string_token(context, lex, TokenType_MACRO, isn_semi, in, index + 1, line_pos);
        }
        break;
      case '\n': 
        lex->curr_line++;
        vector_object_add(lex->lines, vector_Token_create(context));
        line_pos = 0;

      default:
        if (isdigit(c)) {
          TokenType t = TokenType_NUMBER;

          for (int i = 0; i < strlen(in) && isdigit(c = u_getc(in, index + i)); i++) {}
          if (!isspace(c) && c != 'f') {
            //TODO: raise error invalid suffix 
          }
          index = lexer_add_string_token(context, lex, TokenType_NUMBER, is_digit, in, index, line_pos);
        } else if (isalpha(c) || c == '_' || c == '?') {
          index = lexer_add_string_token(context, lex, TokenType_ID, is_idchar, in, index, line_pos);
        } else if (isspace(c)){
          continue;
        } else {
          die("%s:%lu:%lu: \x1b[31merror:\x1b[0m unrecognized token \'%c\'\n", lex->filename, lex->curr_line + 1, line_pos, c);
        }
    }
#undef X
#undef TOKEN_SIZE
#undef T

    arena_free(b);
  }

  Token eof = { TokenType_EOF, NULL, 0, 0 };
  lexer_add_token(context, lex, &eof);

  arena_free(a);
  return 0;
}

const char* lexer_get_filename(Lexer* l)
{
  return l->filename;
}

// string* lexer_get_value(Arena* context, Lexer* lex, size_t line, size_t index)
// {
//   if (index > lex->lines->len) {
//     printf("lexer_get_value_at_index(context = %p, lex = %p, index = %lu): index value too large\n", context, context, index);
//     return u_strnew(context, "");
//   }
//   TokenType t = lex->lines->values[line]->values[index].type;
//   switch(t)
//   {
//     case TokenType_ID: case TokenType_NUMBER: case TokenType_HEX: case TokenType_OCTAL: case TokenType_BINARY: {
//       const string* value = lex->lines->values[line].values[index].value;
//       return u_strslice(context, value, strlen(token_type_str[t]) + 2, u_strlen(value));
//     }
//     default:
//       return NULL;
//   }
// }

// TokenType lexer_get_token_type(Lexer* lex, size_t line_no, size_t index)
// {
//   if (line_no > lex->lines->len) {
//     die("error: lexer_get_token_type_at_index(lex = %p, line = %lu, index = %lu): line out of range\n", lex, line_no, index);
//   }
//   const Line* line = &lex->lines->values[index];
//   if (index > line->len) {
//     die("error: lexer_get_token_type_at_index(lex = %p, line = %lu, index = %lu): index out of range\n", lex, line, index);
//   }
//   if (index == lex->lines->len) return TokenType_EOF;
//   return line->values[index].type;
// }

const Token* lexer_get_token(Lexer* lex, size_t line_no, size_t index)
{
  Line* line = lex->lines->values[line_no];
  return &line->values[index];
}

size_t lexer_get_num_lines(Lexer* lex)
{
  return lex->lines->len;
}

size_t lexer_get_line_len(Lexer* lex, size_t line_no)
{
  Line* line = lex->lines->values[line_no];
  return line->len;
}

string* lexer_to_string(Arena* context, Lexer* lex)
{
  Arena* a = arena_create();
  // char *builder = arena_alloc(a, lex->strlen + 6);
  string* out = NULL;
  for (size_t i = 0; i < lex->lines->len; i++) {
    string* line_str = line_to_string(a, lex->lines->values[i]);
    if (line_str != NULL) {
      char line_prefix[30];
      snprintf(line_prefix, 30, "%lu:\t", i + 1);
      if (out == NULL) out = u_strnew(a, line_prefix);
      else out = u_strcats(a, out, line_prefix);
      out = u_strcat(a, out, line_str);
    }
  }

  out = u_strcpyar(context, out);
  arena_free(a);
  return out;
}

string* line_to_string(Arena* context, const Line* line)
{
  if (line == NULL || line->len == 0) return NULL;

  Arena* a = arena_create();
  string* out = NULL;

  for (size_t i = 0; i < line->len; i++)
  {
    string* token_str = token_to_string(a, &line->values[i]);
    if (out == NULL) out = token_str;
    else out = u_strcat(a, out, token_str);
  }

  out = u_strcats(context, out, "\n");
  arena_free(a);
  return out;
}

Token* token_create(Arena* arena, TokenType t, const string* value, size_t start, size_t end)
{
  Token* new = arena_alloc(arena, sizeof(Token));
  new->type = t;
  new->value = value;
  new->start = start;
  new->end = end;
  return new;
}

string* token_to_string(Arena* context, Token* token)
{
  Arena* a = arena_create();
  string* out = u_strnew(a, "(");
  out = u_strcats(a, out, token_type_str[token->type]);
  if (token->value != NULL) {
    out = u_strcats(a, out, ",");
    out = u_strcat(a, out, token->value);
  }
  out = u_strcats(a, out, ")");

  out = u_strcpyar(context, out);
  arena_free(a);
  return out;
}
