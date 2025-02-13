#ifndef _TOKEN_H
#define _TOKEN_H
#define LOWEST_VALUE
#include "arena_hash_table.c"
#define color(C) "\033[0;3" #C "m"
#define end_color "\033[0m"
#define NEW_TOKEN(TYPE, LITERAL)                                               \
  (Token) { .type = TYPE, .literal = LITERAL }
typedef enum {
  // error handling
  ILLEGAL,
  EOF_,

  // identifier + literals
  IDENTIFIER,

  // operators
  ASSIGN,
  PLUS,
  MINUS,
  SLASH,
  ASTERISK,

  // relationals
  EQUALS,
  BANG,
  NOT_EQUALS,
  LT,
  GT,
  L_EQUALS,
  G_EQUALS,

  // delimiter
  COMMA,
  SEMICOLON,

  L_PAREN,
  R_PAREN,
  L_BRACE,
  R_BRACE,

  // data_types
  INT,
  // keywords
  LET,
  FUNCTION,
  IF,
  ELSE,
  TRUE,
  FALSE,
  RETURN,
} TokenType;

KeyValue TYPES_ARR[] = {
    kv("ILLEGAL", ILLEGAL), // parsing stuff
    kv("", EOF_),           //
                            // operators
    kv("=", ASSIGN),        //
    kv("-", MINUS),          //
    kv("+", PLUS),          //
    kv("*", ASTERISK),      //
    kv("/", SLASH),         //
                            // relationals
    kv("==", EQUALS),       //
    kv("!", BANG),          //
    kv("!=", NOT_EQUALS),   //
    kv("<", LT),            //
    kv(">", GT),            //
    kv(">=", G_EQUALS),     //
    kv("<=", L_EQUALS),     //
                            //
    kv(",", COMMA),         // delimiters
    kv(";", SEMICOLON),     //
    kv("(", L_PAREN),       //
    kv(")", R_PAREN),       //
    kv("{", L_BRACE),       //
    kv("}", R_BRACE),       //
                            // keywords
    kv("fn", FUNCTION),     //
    kv("let", LET),         //
    kv("if", IF),           //
    kv("else", ELSE),       //
    kv("true", TRUE),       //
    kv("false", FALSE),     //
    kv("return", RETURN),   //
}; //

#define TYPES_LEN (sizeof TYPES_ARR / sizeof TYPES_ARR[0])

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
  switch (token_type) {
  case IDENTIFIER:
    return "identifier";
  case INT:
    return "int";
  default:
    return hash_table_find_key(TYPES, token_type);
  }
}

void print_token(const char *title, Token token) {
  const char *token_literal = get_token_literal(token.type);
  printf("%s:\n"
         "\tliteral:" color(5) "`%s`" end_color "\n"
                               "\ttype:" color(6) "`%s`" end_color "\n",
         title, token.literal.str, token_literal);
  printf("\n");
}

#endif /* ifndef _TOKEN_H */
