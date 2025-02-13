#include "lexer.c"
#include "token.c"
#include <assert.h>
#include <stdio.h>

char *ts[] = {
    // error handling
    "ILLEGAL", "EOF_", "IDENTIFIER", "INT",
    // operators
    "ASSIGN", "PLUS", "MINUS",
    // delimiter
    "COMMA", "SEMICOLON", "L_PAREN", "R_PAREN", "L_BRACE", "R_BRACE",
    // keywords
    "FUNCTION", "LET"};

#define color(C) "\033[0;3" #C "m"
#define end_color "\033[0m"

// Very small test helpers
int failed = 0;
#define TEST(name) void name()
#define RUN_TEST(name)                                                         \
  printf("\n\033[1m%s\n\033[0m", #name);                                       \
  name()
#define ASSERT(expr)                                                           \
  if (!(expr)) {                                                               \
    failed = 1;                                                                \
    printf("\033[0;31mFAIL: %s\n\033[0m", #expr);                              \
  } else {                                                                     \
    printf("\033[0;32mPASS: %s\n\033[0m", #expr);                              \
  }

#define ASSERT_TYPES(TOK, EXPECTED_TYPE, TYPE)                                 \
  if (EXPECTED_TYPE == TYPE) {                                                 \
    printf("\033[0;32mPASS: %s == %s\n\033[0m", TOK.literal.str,               \
           ts[EXPECTED_TYPE]);                                                 \
  } else {                                                                     \
    failed = 1;                                                                \
    printf("\033[0;31mFAIL: %s != %s\n\033[0m", TOK.literal.str,               \
           ts[EXPECTED_TYPE]);                                                 \
  }

#define ASSERT_STR_EQ(str1, str2)                                              \
  if (string_equals(str1, str2)) {                                             \
    printf("\033[0;32mPASS: %s == %s\n\033[0m", str1.str, str2.str);           \
  } else {                                                                     \
    failed = 1;                                                                \
    printf("\033[0;31mFAIL: %s != %s\n\033[0m", str1.str, str2.str);           \
  }
#define MACRO
// End of test helpers

TEST(test_string_assert) {
  ASSERT_STR_EQ(string("hello"), string("helloasdf"));
}

/*
FAIL: let != let
PASS: five == five
PASS: = == =
FAIL: (null) != 5
FAIL: 5 != ;
FAIL:  !=
 * */

TEST(test_tokens) {
  Arena arena = (Arena){.begin = NULL, .end = NULL};
  String input = arena_new_string(&arena, "&++){},;");
  /*String input = string("{}");*/
  Lexer lexi = lexer_new_lexer(input);

  Token tokens_test[] = {
      (Token){.type = ILLEGAL, .literal = string("ileg")},
      (Token){.type = PLUS, .literal = string("+")},
      (Token){.type = PLUS, .literal = string("+")},
      (Token){.type = R_PAREN, .literal = string(")")},
      (Token){.type = L_BRACE, .literal = string("{")},
      (Token){.type = R_BRACE, .literal = string("}")},
      (Token){.type = COMMA, .literal = string(",")},
      (Token){.type = SEMICOLON, .literal = string(";")},
      (Token){.type = EOF_, .literal = string("")},
  };

  for (int i = 0; i < (sizeof tokens_test / sizeof tokens_test[0]); i++) {
    Token tok = lexer_next_token(&arena, &lexi);
    Token expected_tok = tokens_test[i];
    TokenType type = tok.type;
    TokenType expected_type = tokens_test[i].type;
    if (expected_type == type) {
      printf("\033[0;32mPASS: %s == %s\n\033[0m", tok.literal.str,
             expected_tok.literal.str);
    } else {
      failed = 1;
      printf("\033[0;31mFAIL: %s != %s\n\033[0m", tok.literal.str,
             expected_tok.literal.str);
    }
  }
}

TEST(test_more_tokens) {
  Arena arena = (Arena){.begin = NULL, .end = NULL};
  String input = arena_new_string(&arena, "let five = 5;"
                                          "let ten = 10;"
                                          "let add = fn(x, y) {"
                                          "x + y;"
                                          "};"
                                          "let result = add(five, ten);");
  Lexer lexi = lexer_new_lexer(input);
  Token tokens_test[] = {
      (Token){.type = LET, .literal = string("let")},
      (Token){.type = IDENTIFIER, .literal = string("five")},
      (Token){.type = ASSIGN, .literal = string("=")},
      (Token){.type = INT, .literal = string("5")},
      (Token){.type = SEMICOLON, .literal = string(";")},
      (Token){.type = LET, .literal = string("let")},
      (Token){.type = IDENTIFIER, .literal = string("ten")},
      (Token){.type = ASSIGN, .literal = string("=")},
      (Token){.type = INT, .literal = string("10")},
      (Token){.type = SEMICOLON, .literal = string(";")},
      (Token){.type = LET, .literal = string("let")},
      (Token){.type = IDENTIFIER, .literal = string("add")},
      (Token){.type = ASSIGN, .literal = string("=")},
      (Token){.type = FUNCTION, .literal = string("fn")},
      (Token){.type = L_PAREN, .literal = string("(")},
      (Token){.type = IDENTIFIER, .literal = string("x")},
      (Token){.type = COMMA, .literal = string(",")},
      (Token){.type = IDENTIFIER, .literal = string("y")},
      (Token){.type = R_PAREN, .literal = string(")")},
      (Token){.type = L_BRACE, .literal = string("{")},
      (Token){.type = IDENTIFIER, .literal = string("x")},
      (Token){.type = PLUS, .literal = string("+")},
      (Token){.type = IDENTIFIER, .literal = string("y")},
      (Token){.type = SEMICOLON, .literal = string(";")},
      (Token){.type = R_BRACE, .literal = string("}")},
      (Token){.type = SEMICOLON, .literal = string(";")},
      (Token){.type = LET, .literal = string("let")},
      (Token){.type = IDENTIFIER, .literal = string("result")},
      (Token){.type = ASSIGN, .literal = string("=")},
      (Token){.type = IDENTIFIER, .literal = string("add")},
      (Token){.type = L_PAREN, .literal = string("(")},
      (Token){.type = IDENTIFIER, .literal = string("five")},
      (Token){.type = COMMA, .literal = string(",")},
      (Token){.type = IDENTIFIER, .literal = string("ten")},
      (Token){.type = R_PAREN, .literal = string(")")},
      (Token){.type = SEMICOLON, .literal = string(";")},
      (Token){.type = EOF_, .literal = string("")},
  };

  for (int i = 0; i < (sizeof tokens_test / sizeof tokens_test[0]); i++) {
    Token tok = lexer_next_token(&arena, &lexi);
    Token expected_tok = tokens_test[i];
    TokenType type = tok.type;
    TokenType expected_type = tokens_test[i].type;
    /*print_token(color(4)"MY_TOKEN"end_color, tok);*/
    /*print_token(color(6)"EXPECTED_TOKEN"end_color, expected_tok);*/
    ASSERT_TYPES(tok, type, expected_type);
  }
}

int main() {
  test_more_tokens();
  return failed;
}
