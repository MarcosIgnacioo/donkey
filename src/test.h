#include "./parser/ast.c"
#include "array.c"
#include "lexer.c"
#include "repl.c"
#include "token.c"
#include <assert.h>
#include <stdio.h>
void test_check_parser_errors(Parser *parser);
int test_expressions();
bool test_identifier(Expression expression, String value);
bool test_integer_literal(Expression exp, I64 value);
bool test_literal_expression(Expression exp, void *value);
bool test_infix_expression(Expression expression, void *left,
                           const char *operator, void * right);
int test_infix_expressions();
int test_infix_expressions_harder();
int test_foo();
int test_prefix_expressions();
int test_expressions_integer_literals();
int test_expressions_beta();
bool test_let_statement(Node statement, String expected_name,
                        void *expected_value);
bool test_return_statement(Node statement, void *value);

#define NUMARGS(...) (sizeof((int[]){__VA_ARGS__}) / sizeof(int))
#define end_program goto exit_program

#define ENUM_TO_STRING(ENUM_NAME, ...)                                         \
  enum ENUM_NAME { __VA_ARGS__ };                                              \
  char ENUM_NAME##_strings[] = #__VA_ARGS__;                                   \
  long ENUM_NAME##strings_indices[NUMARGS(__VA_ARGS__)];                       \
  char *ENUM_NAME##_to_string(enum ENUM_NAME value) {                          \
    static int init = 0;                                                       \
    if (init == 0) {                                                           \
      int n = 0;                                                               \
      ENUM_NAME##strings_indices[n++] = 0;                                     \
      char *curr_pos = strchr(ENUM_NAME##_strings, ',');                       \
      while (curr_pos) {                                                       \
        *curr_pos = '\0';                                                      \
        ENUM_NAME##strings_indices[n++] = (++curr_pos - ENUM_NAME##_strings);  \
        curr_pos = strchr(curr_pos, ',');                                      \
      }                                                                        \
      init++;                                                                  \
    }                                                                          \
    return (char *)ENUM_NAME##_strings + ENUM_NAME##strings_indices[value];    \
  }

#define color(C) "\033[0;3" #C "m"
#define stringify(VAR) #VAR
#define end_color "\033[0m"
#define LOG_ERROR color(3) "[TEST ERROR] : " end_color
#define LOG_SUCCESS color(2) "[SUCCESS] : " end_color
#define LOG_INFO color(4) "[TEST INFO] : " end_color
#define print_error(...) printf(LOG_ERROR __VA_ARGS__)

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
           get_token_literal(expected_type));                                  \
  } else {                                                                     \
    failed = 1;                                                                \
    printf("\033[0;31mFAIL: %s != %s\n\033[0m", TOK.literal.str,               \
           (const char *)TYPES_ARR[EXPECTED_TYPE].key);                        \
  }

#define ASSERT_STR_EQ(str1, str2)                                              \
  if (string_equals(str1, str2)) {                                             \
    printf("\033[0;32mPASS: %s == %s\n\033[0m", str1.str, str2.str);           \
  } else {                                                                     \
    failed = 1;                                                                \
    printf("\033[0;31mFAIL: %s != %s\n\033[0m", str1.str, str2.str);           \
  }
TEST(test_string_assert) {
  ASSERT_STR_EQ(string("hello"), string("helloasdf"));
}
