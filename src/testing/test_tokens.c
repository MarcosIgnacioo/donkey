#ifndef _TEST_TOKENS_H
#include "./test.h"

typedef struct {
  char *input;
  I64 expected;
} TestResultToken;

void test_token() {
  Arena arena = (Arena){.begin = NULL, .end = NULL};
  String input = arena_new_string(&arena, "\"hello\"");
  /*String input = string("{}");*/
  Lexer lexi = lexer_new_lexer(input);
  bool failed = true;

  Token tokens_test[] = {
      (Token){.type = STRING, .literal = string("hello")},
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
  (void) failed;
}


#endif /* ifndef _TEST_TOKENS_H */
