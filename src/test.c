#include "lexer.c"
#include "token.c"
#include <assert.h>
#include <stdio.h>

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
#define ASSERT_STR_EQ(str1, str2)                                              \
  if (string_equals(str1, str2)) {                                             \
    printf("\033[0;32mPASS: %s == %s\n\033[0m", str1.str, str2.str);           \
  } else {                                                                     \
    failed = 1;                                                                \
    printf("\033[0;31mFAIL: %s != %s\n\033[0m", str1.str, str2.str);           \
  }
// End of test helpers

TEST(test_string_assert) {
  ASSERT_STR_EQ(string("hello"), string("helloasdf"));
}

typedef struct {
  TokenType expected_type;
  String expected_literal;
} TokenTest;

TEST(test_tokens) {
  Arena arena = (Arena){.begin = NULL, .end = NULL };
  String input = arena_new_string(&arena, "=+(){},;");
  /*String input = string("{}");*/
  Lexer lexi = lexer_new_lexer(input);

  TokenTest tokens_test[] = {
      (TokenTest){.expected_type = ASSIGN, .expected_literal = string("=")},
      (TokenTest){.expected_type = PLUS, .expected_literal = string("+")},
      (TokenTest){.expected_type = L_PAREN, .expected_literal = string("(")},
      (TokenTest){.expected_type = R_PAREN, .expected_literal = string(")")},
      (TokenTest){.expected_type = L_BRACE, .expected_literal = string("{")},
      (TokenTest){.expected_type = R_BRACE, .expected_literal = string("}")},
      (TokenTest){.expected_type = COMMA, .expected_literal = string(",")},
      (TokenTest){.expected_type = SEMICOLON, .expected_literal = string(";")},
      (TokenTest){.expected_type = EOF_, .expected_literal = string("")},
  };

  for (int i = 0; i < (sizeof tokens_test / sizeof tokens_test[0]); i++) {
    Token tok = lexer_next_token(&arena, &lexi);
    TokenType type = tok.type;
    TokenType expected_type = tokens_test[i].expected_type;
    ASSERT_STR_EQ(string(TYPES[type]), string(TYPES[expected_type]));
  }
}

TEST(test_more_tokens) {
  Arena arena = (Arena){.begin = NULL, .end = NULL };
  String input = arena_new_string(&arena, "let five = 5;");
  /*String input = string("{}");*/
  Lexer lexi = lexer_new_lexer(input);

  TokenTest tokens_test[] = {
      (TokenTest){.expected_type = LET, .expected_literal = string("let")},
      (TokenTest){.expected_type = IDENTIFIER,
                  .expected_literal = string("five")},
      (TokenTest){.expected_type = ASSIGN, .expected_literal = string("=")},
      (TokenTest){.expected_type = INT, .expected_literal = string("5")},
      (TokenTest){.expected_type = SEMICOLON, .expected_literal = string(";")},
      /*(TokenTest){.expected_type = LET, .expected_literal = string("let")},*/
      /*(TokenTest){.expected_type = IDENTIFIER, .expected_literal =
         string("ten")},*/
      /*(TokenTest){.expected_type = ASSIGN, .expected_literal = string("=")},*/
      /*(TokenTest){.expected_type = INT, .expected_literal = string("10")},*/
      /*(TokenTest){.expected_type = SEMICOLON, .expected_literal =
         string(";")},*/
      /*(TokenTest){.expected_type = LET, .expected_literal = string("let")},*/
      /*(TokenTest){.expected_type = IDENTIFIER, .expected_literal =
         string("add")},*/
      /*(TokenTest){.expected_type = ASSIGN, .expected_literal = string("=")},*/
      /*(TokenTest){.expected_type = FUNCTION, .expected_literal =
         string("fn")},*/
      /*(TokenTest){.expected_type = L_PAREN, .expected_literal =
         string("(")},*/
      /*(TokenTest){.expected_type = IDENTIFIER, .expected_literal =
         string("x")},*/
      /*(TokenTest){.expected_type = COMMA, .expected_literal = string(",")},*/
      /*(TokenTest){.expected_type = IDENTIFIER, .expected_literal =
         string("y")},*/
      /*(TokenTest){.expected_type = R_PAREN, .expected_literal =
         string(")")},*/
      /*(TokenTest){.expected_type = L_BRACE, .expected_literal =
         string("{")},*/
      /*(TokenTest){.expected_type = IDENTIFIER, .expected_literal =
         string("x")},*/
      /*(TokenTest){.expected_type = PLUS, .expected_literal = string("+")},*/
      /*(TokenTest){.expected_type = IDENTIFIER, .expected_literal =
         string("y")},*/
      /*(TokenTest){.expected_type = SEMICOLON, .expected_literal =
         string(";")},*/
      /*(TokenTest){.expected_type = R_BRACE, .expected_literal =
         string("}")},*/
      /*(TokenTest){.expected_type = SEMICOLON, .expected_literal =
         string(";")},*/
      /*(TokenTest){.expected_type = LET, .expected_literal = string("let")},*/
      /*(TokenTest){.expected_type = IDENTIFIER,*/
      /*            .expected_literal = string("result")},*/
      /*(TokenTest){.expected_type = ASSIGN, .expected_literal = string("=")},*/
      /*(TokenTest){.expected_type = IDENTIFIER, .expected_literal =
         string("add")},*/
      /*(TokenTest){.expected_type = L_PAREN, .expected_literal =
         string("(")},*/
      /*(TokenTest){.expected_type = IDENTIFIER, .expected_literal =
         string("five")},*/
      /*(TokenTest){.expected_type = COMMA, .expected_literal = string(",")},*/
      /*(TokenTest){.expected_type = IDENTIFIER, .expected_literal =
         string("ten")},*/
      /*(TokenTest){.expected_type = R_PAREN, .expected_literal =
         string(")")},*/
      /*(TokenTest){.expected_type = SEMICOLON, .expected_literal =
         string(";")},*/
      (TokenTest){.expected_type = EOF, .expected_literal = string("")},
  };

  for (int i = 0; i < (sizeof tokens_test / sizeof tokens_test[0]); i++) {
    Token tok = lexer_next_token(&arena, &lexi);
    TokenType type = tok.type;
    TokenType expected_type = tokens_test[i].expected_type;
    ASSERT_STR_EQ(string(TYPES[type]), string(TYPES[expected_type]));
  }
}

int main() {
  RUN_TEST(test_tokens);
  RUN_TEST(test_more_tokens);
  return failed;
}
