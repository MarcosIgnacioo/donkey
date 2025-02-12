#ifndef _LEXER_H
#define _LEXER_H
#include "arena.c"
#include "arena_strings.c"
#include "token.c"
#include <stddef.h>
#include <stdint.h>
// char *tokens_arena
// char *lexer_arena
// char *parsing_arena
// in a rewrite should do like a union this time i think
typedef struct {
  String input;
  unsigned int position;
  unsigned int next_position;
  char current_char;
} Lexer;
typedef bool (*PredicatorFN)(char);

// public
Lexer lexer_new_lexer(String input);
Token lexer_next_token(Arena *arena, Lexer *lexer);
bool _lexer_is_valid_letter(char c);
bool _lexer_is_valid_number(char c);
// hidden
void _lexer_read_char(Lexer *lexer);

bool _lexer_is_valid_letter(char c) {
  return (c >= 'A' && c <= 'z') || c == '_';
}
bool _lexer_is_valid_number(char c) {
  return (c >= '0' && c <= '9') || c == '.';
}

String _lexer_read_identifier(Arena *arena, Lexer *lexer);

String _lexer_read_blank(Arena *arena, Lexer *lexer,
                         PredicatorFN predicator_fn);

Lexer lexer_new_lexer(String input) {
  Lexer lexer = (Lexer){
      .input = input,     //
      .position = 0,      //
      .next_position = 0, //
      .current_char = 0   //
  };
  _lexer_read_char(&lexer);
  return lexer;
}

void _lexer_read_char(Lexer *lexer) {
  if (lexer->next_position >= lexer->input.len) {
    lexer->current_char = 0;
  } else {
    lexer->current_char = lexer->input.str[lexer->next_position];
  }
  /*char *read_identifier() STR_WITH_LEN(lexer -> current_char, )*/
  lexer->position = lexer->next_position;
  lexer->next_position++;
}

Token lexer_next_token(Arena *arena, Lexer *lexer) {
  char c = lexer->current_char;
  Token tok = (Token){0};
  switch (c) {
  case '\0':
  case '=':
  case '+':
  case ',':
  case ';':
  case '(':
  case ')':
  case '{':
  case '}': {
    TokenType type = get_token_type_of_char(c);
    const char *str_slice = lexer->input.str + lexer->position;
    String str = arena_new_string_with_len(arena, str_slice, 2);
    tok = NEW_TOKEN(type, str);
    break;
  }
  default: {
    if (_lexer_is_valid_letter(c)) {
      String identifier =
          _lexer_read_blank(arena, lexer, &_lexer_is_valid_letter);
      TokenType type = get_token_type_of_char(c);
      if (type == ILLEGAL) {
        type = IDENTIFIER;
      }
      tok = NEW_TOKEN(type, identifier);
    } else if (_lexer_is_valid_number(c)) {
      String identifier =
          _lexer_read_blank(arena, lexer, &_lexer_is_valid_number);
      tok = NEW_TOKEN(INT, identifier);
    }
  }
  }
  _lexer_read_char(lexer);
  return tok;
}

String _lexer_read_identifier(Arena *arena, Lexer *lexer) {
  U64 start = lexer->position;
  while (_lexer_is_valid_letter(lexer->current_char)) {
    _lexer_read_char(lexer);
  }
  U64 end = lexer->position;
  return arena_new_string_with_len(arena, lexer->input.str + start, end - start);
}

String _lexer_read_blank(Arena *arena, Lexer *lexer,
                         PredicatorFN predicator_fn) {
  U64 start = lexer->position;
  while (predicator_fn(lexer->current_char)) {
    _lexer_read_char(lexer);
  }
  U64 end = lexer->position;
  return arena_new_string_with_len(arena, lexer->input.str + start, end - start);
}

#endif /* ifndef _LEXER_H */
