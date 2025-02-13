#ifndef _TOKEN_H
#define _TOKEN_H
#define LOWEST_VALUE
#include "arena_hash_table.c"
#define NEW_TOKEN(TYPE, LITERAL)                                               \
  (Token) { .type = TYPE, .literal = LITERAL }
#define TYPES_LEN 15
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
  MINUS,

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

KeyValue TYPES_ARR[] = {
    kv("ILLEGAL", ILLEGAL),       //
    kv("", EOF_),                 //
    kv("IDENTIFIER", IDENTIFIER), //
    kv("INT", INT),               //
    kv("=", ASSIGN),              //
    kv("+", PLUS),                //
    kv("-", MINUS),               //
    kv(",", COMMA),               //
    kv(";", SEMICOLON),           //
    kv("(", L_PAREN),             //
    kv(")", R_PAREN),             //
    kv("{", L_BRACE),             //
    kv("}", R_BRACE),             //
    kv("fn", FUNCTION),           //
    kv("let", LET),               //
}; //

HashTable TYPES =
    (HashTable){.len = TYPES_LEN, .capacity = TYPES_LEN, .items = TYPES_ARR};

typedef struct {
  TokenType type;
  String literal;
} Token;

// todo cambiar a algo que no sea O(N)
// probablemente sea un hashmap
TokenType get_token_type(String input) {
  return (TokenType)hash_table_find_item(TYPES, input);
}

const char *get_token_literal(TokenType token_type) {
  return hash_table_find_key(TYPES, token_type);
}

void print_token(const char *title, Token token) {
  printf("%s:\n"
         "\tliteral:`%s`\n"
         "\ttype:`%s`\n",
         title, token.literal.str, TYPES_ARR[token.type].key);
}

#endif /* ifndef _TOKEN_H */
