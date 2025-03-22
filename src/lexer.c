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
  char next_char;
} Lexer;
typedef bool (*PredicatorFN)(char);

// public
Lexer lexer_new_lexer(String input);
Token lexer_next_token(Arena *arena, Lexer *lexer);
bool _lexer_is_valid_letter(char c);
bool _lexer_is_valid_number(char c);

// hidden
void _lexer_read_char(Lexer *lexer);

void _lexer_skip_white_space(Lexer *lexer);

bool _lexer_is_valid_letter(char c) {
  return ((c >= 'A' && c <= 'z') || c == '_') && (c != '[' && c != ']');
}

bool _lexer_is_valid_string_char(char c) { return c != '"' && c != '\0'; }

bool _lexer_is_valid_number(char c) {
  return (c >= '0' && c <= '9') || c == '.';
}
String _lexer_read_blank(Arena *arena, Lexer *lexer,
                         PredicatorFN predicator_fn);
String _lexer_read_string(Arena *arena, Lexer *lexer,
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
    lexer->next_char = 0;
    lexer->current_char = 0;
  } else {
    // TODO: Asegurarnos aqui que no nos estmaos saliendo d los bounds de manera
    // mas sexy
    if (lexer->next_position + 1 < lexer->input.len) {
      lexer->next_char = lexer->input.str[lexer->next_position + 1];
    } else {
      lexer->next_char = 0;
    }
    lexer->current_char = lexer->input.str[lexer->next_position];
  }
  /*char *read_identifier() STR_WITH_LEN(lexer -> current_char, )*/
  lexer->position = lexer->next_position;
  lexer->next_position++;
}

Token lexer_next_token(Arena *arena, Lexer *lexer) {
  _lexer_skip_white_space(lexer);
  char c = lexer->current_char;
  Token tok = (Token){0};
  U64 default_size = 1;
  switch (c) {
  case '"': {
    _lexer_read_char(lexer);
    String literal =
        _lexer_read_string(arena, lexer, &_lexer_is_valid_string_char);
    TokenType type = STRING;
    tok = NEW_TOKEN(type, literal);
    break;
  }
  case '!':
  case '<':
  case '>':
  case '=': {
    if (lexer->next_char == '=') {
      default_size = 2;
    }
  }
  case '\0': // eof
  case '-':  // operators
  case '+':
  case '*':
  case '/':
  case ',': // delimiters
  case ';':
  case ':':
  case '[':
  case ']':
  case '(':
  case ')':
  case '{':
  case '}':
    //
    {
      const char *str_slice = lexer->input.str + lexer->position;
      String str = arena_new_string_with_len(arena, str_slice, default_size);
      TokenType type = get_token_type(str);
      tok = NEW_TOKEN(type, str);
      // ver si hay una manera de que se pueda hacer esto sin bueno hacer esto
      if (default_size > 1) {
        _lexer_read_char(lexer);
      }
      break;
    }
  default:
    //
    {
      if (_lexer_is_valid_letter(c)) {
        String literal =
            _lexer_read_blank(arena, lexer, &_lexer_is_valid_letter);
        TokenType type = get_token_type(literal);
        if (type == ILLEGAL) {
          type = IDENTIFIER;
        }
        tok = NEW_TOKEN(type, literal);
      } else if (_lexer_is_valid_number(c)) {
        String number =
            _lexer_read_blank(arena, lexer, &_lexer_is_valid_number);
        tok = NEW_TOKEN(INT, number);
      } else {
        tok = NEW_TOKEN(ILLEGAL, arena_new_string(arena, &c));
      }
    }
  }
  _lexer_read_char(lexer);
  _lexer_skip_white_space(lexer);
  return tok;
}

void _lexer_skip_white_space(Lexer *lexer) {
  while (                            //
      lexer->current_char == ' ' ||  //
      lexer->current_char == '\t' || //
      lexer->current_char == '\r' || //
      lexer->current_char == '\n') {
    _lexer_read_char(lexer);
  }
}

/*Why i used next_char?*/
// here we do this gooofy stuff because at the end of
// the switch of the lexer_next_token we are always
// doing _lexer_read_char at the end
// which 'eats up' our `;` because when we read 123; (in the old way)
// the lexer is stopped at the `;`, and because of that, when we read in the
// lexer_next_token at the end well we just skip it
// we could just do an early return in that function
// but [redacted]
String _lexer_read_blank(Arena *arena, Lexer *lexer,
                         PredicatorFN predicator_fn) {
  U64 start = lexer->position;
  while (predicator_fn(lexer->next_char)) {
    _lexer_read_char(lexer);
  }
  U64 end = lexer->next_position;
  String ret =
      arena_new_string_with_len(arena, lexer->input.str + start, end - start);
  return ret;
}

String _lexer_read_string(Arena *arena, Lexer *lexer,
                          PredicatorFN predicator_fn) {
  U64 start = lexer->position;
  char c = lexer->current_char;
  while (true) {
    c = lexer->current_char;
    if (c == '"' || c == '\0') {
      break;
    }
    _lexer_read_char(lexer);
  }
  U64 end = lexer->position;
  String ret =
      arena_new_string_with_len(arena, lexer->input.str + start, end - start);
  return ret;
}

#endif /* ifndef _LEXER_H */
