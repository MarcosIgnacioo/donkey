#ifndef _TOKEN_H
#define _TOKEN_H
#define LOWEST_VALUE
#define NEW_TOKEN(TYPE, LITERAL)                                               \
  (Token) { .type = TYPE, .literal = LITERAL }

char *TYPES[] = {
    "ILLEGAL",    // 0
    "EOF_",       // 1
    "IDENTIFIER", // 2
    "INT",        // 3
    "=",          // 4
    "+",          // 5
    ",",          // 6
    ";",          // 7
    "(",          // 8
    ")",          // 9
    "{",          // 10
    "}",          // 11
    "FUNCTION",   // 12
    "LET"         // 13
};

typedef enum {
  // error handling
  ILLEGAL,
  EOF_,

  // identifier + literals
  IDENTIFIER,
  INT,

  // operators
  ASSIGN,
  PLUS,

  // delimiter
  COMMA,
  SEMICOLON,

  L_PAREN,
  R_PAREN,
  L_BRACE,
  R_BRACE,

  // keywords
  FUNCTION,
  LET,
} TokenType;

typedef struct {
  TokenType type;
  String literal;
} Token;

// todo cambiar a algo que no sea O(N)
// probablemente sea un hashmap
TokenType get_token_type_of_char(char c) {
  TokenType tok = ILLEGAL;
  int len = sizeof TYPES / sizeof TYPES[0];
  for (tok = 0; tok < (len); tok++) {

    if (*TYPES[tok] == c) {
      break;
    }
  }
  return tok % (len - 1);
}

#endif /* ifndef _TOKEN_H */
