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
    kv(KeyValue, "ILLEGAL", ILLEGAL), // parsing stuff
    kv(KeyValue, "", EOF_),           //
                                      // operators
    kv(KeyValue, "=", ASSIGN),        //
    kv(KeyValue, "-", MINUS),         //
    kv(KeyValue, "+", PLUS),          //
    kv(KeyValue, "*", ASTERISK),      //
    kv(KeyValue, "/", SLASH),         //
                                      // relationals
    kv(KeyValue, "==", EQUALS),       //
    kv(KeyValue, "!", BANG),          //
    kv(KeyValue, "!=", NOT_EQUALS),   //
    kv(KeyValue, "<", LT),            //
    kv(KeyValue, ">", GT),            //
    kv(KeyValue, ">=", G_EQUALS),     //
    kv(KeyValue, "<=", L_EQUALS),     //
                                      //
    kv(KeyValue, ",", COMMA),         // delimiters
    kv(KeyValue, ";", SEMICOLON),     //
    kv(KeyValue, "(", L_PAREN),       //
    kv(KeyValue, ")", R_PAREN),       //
    kv(KeyValue, "{", L_BRACE),       //
    kv(KeyValue, "}", R_BRACE),       //
                                      // keywords
    kv(KeyValue, "fn", FUNCTION),     //
    kv(KeyValue, "let", LET),         //
    kv(KeyValue, "if", IF),           //
    kv(KeyValue, "else", ELSE),       //
    kv(KeyValue, "true", TRUE),       //
    kv(KeyValue, "false", FALSE),     //
    kv(KeyValue, "return", RETURN),   //
}; //

#define TYPES_LEN (sizeof TYPES_ARR / sizeof TYPES_ARR[0])

bool compare_string_keys(void *a, void *b) {
  const char *a_key = (*(KeyValue *)a).key;
  const char *b_key = (*(String *)b).str;
  return c_string_equals(a_key, b_key);
}

bool compare_token_type_values(void *a, void *b) {
  TokenType a_key = (*(KeyValue *)a).value;
  TokenType b_key = *(TokenType *)b;
  return a_key == b_key;
}

HashTable TYPES = (HashTable){
    .len = TYPES_LEN,                                //
    .capacity = TYPES_LEN,                           //
    .items = TYPES_ARR,                              //
    .are_keys_equals = &compare_string_keys,         //
    .are_values_equals = &compare_token_type_values, //
    .item_size = sizeof(KeyValue)                    //
};

typedef struct {
  TokenType type;
  String literal;
} Token;

// todo cambiar a algo que no sea O(N)
// probablemente sea un hashmap
TokenType get_token_type(String input) {
  KeyValue *kv = hash_table_find_item(TYPES, &input);
  if (kv) {
    return (TokenType)(kv)->value;
  } else {
    return ILLEGAL;
  }
}

const char *get_token_literal(TokenType token_type) {
  switch (token_type) {
  case IDENTIFIER:
    return "identifier";
  case INT:
    return "int";
  default: {
    KeyValue *kv = (KeyValue *)hash_table_find_key(TYPES, &token_type);
    return kv->key;
  }
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
