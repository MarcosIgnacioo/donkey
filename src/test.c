#include "./parser/ast.c"
#include "array.c"
#include "lexer.c"
#include "repl.c"
#include "token.c"
#include <assert.h>
#include <stdio.h>

#define NUMARGS(...) (sizeof((int[]){__VA_ARGS__}) / sizeof(int))

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
#define LOG_ERROR color(1) "[ERROR] : " end_color
#define LOG_SUCCESS color(2) "[SUCCESS] : " end_color

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
  String input = arena_new_string(&arena, "++(){},;");
  /*String input = string("{}");*/
  Lexer lexi = lexer_new_lexer(input);

  Token tokens_test[] = {
      (Token){.type = PLUS, .literal = string("+")},
      (Token){.type = PLUS, .literal = string("+")},
      (Token){.type = L_PAREN, .literal = string("(")},
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
                                          "x * y;"
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
      (Token){.type = ASTERISK, .literal = string("*")},
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
    TokenType type = tok.type;
    TokenType expected_type = tokens_test[i].type;
    /*print_token(color(4)"MY_TOKEN"end_color, tok);*/
    /*print_token(color(6)"EXPECTED_TOKEN"end_color, expected_tok);*/
    ASSERT_TYPES(tok, type, expected_type);
    printf("\n");
  }
}

TEST(test_more_more_tokens) {
  Arena arena = (Arena){.begin = NULL, .end = NULL};
  String input = arena_new_string(&arena, "let five = 5;\n"
                                          "let ten = 10;\n"
                                          "let add = fn(x, y) {\n"
                                          "x + y;\n"
                                          "};\n"
                                          "let result = add(five, ten);\n"
                                          "!-/*5;\n"
                                          "5 < 10 > 5;\n"
                                          "if (5 < 10) {\n"
                                          "return true;\n"
                                          "} else {\n"
                                          "return false;\n"
                                          "}"
                                          "5 <= 10 >= 5;\n"
                                          "5 != 10 == 5;\n");
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

      (Token){.type = BANG, .literal = string("!")},
      (Token){.type = MINUS, .literal = string("-")},
      (Token){.type = SLASH, .literal = string("/")},
      (Token){.type = ASTERISK, .literal = string("*")},
      (Token){.type = INT, .literal = string("5")},
      (Token){.type = SEMICOLON, .literal = string(";")},

      (Token){.type = INT, .literal = string("5")},
      (Token){.type = LT, .literal = string("<")},
      (Token){.type = INT, .literal = string("10")},
      (Token){.type = GT, .literal = string(">")},
      (Token){.type = INT, .literal = string("5")},
      (Token){.type = SEMICOLON, .literal = string(";")},

      (Token){.type = IF, .literal = string("if")},
      (Token){.type = L_PAREN, .literal = string("(")},
      (Token){.type = INT, .literal = string("5")},
      (Token){.type = LT, .literal = string("<")},
      (Token){.type = INT, .literal = string("10")},
      (Token){.type = R_PAREN, .literal = string(")")},
      (Token){.type = L_BRACE, .literal = string("{")},
      (Token){.type = RETURN, .literal = string("return")},
      (Token){.type = TRUE, .literal = string("true")},
      (Token){.type = SEMICOLON, .literal = string(";")},
      (Token){.type = R_BRACE, .literal = string("}")},
      (Token){.type = ELSE, .literal = string("else")},
      (Token){.type = L_BRACE, .literal = string("{")},
      (Token){.type = RETURN, .literal = string("return")},
      (Token){.type = FALSE, .literal = string("false")},
      (Token){.type = SEMICOLON, .literal = string(";")},
      (Token){.type = R_BRACE, .literal = string("}")},

      (Token){.type = INT, .literal = string("5")},
      (Token){.type = L_EQUALS, .literal = string("<=")},
      (Token){.type = INT, .literal = string("10")},
      (Token){.type = G_EQUALS, .literal = string(">=")},
      (Token){.type = INT, .literal = string("5")},
      (Token){.type = SEMICOLON, .literal = string(";")},

      (Token){.type = INT, .literal = string("5")},
      (Token){.type = NOT_EQUALS, .literal = string("!=")},
      (Token){.type = INT, .literal = string("10")},
      (Token){.type = EQUALS, .literal = string("==")},
      (Token){.type = INT, .literal = string("5")},
      (Token){.type = SEMICOLON, .literal = string(";")},

      (Token){.type = EOF_, .literal = string("")},
  };

  for (int i = 0; i < (sizeof tokens_test / sizeof tokens_test[0]); i++) {
    Token tok = lexer_next_token(&arena, &lexi);
    TokenType type = tok.type;
    TokenType expected_type = tokens_test[i].type;
    /*print_token(color(4)"MY_TOKEN"end_color, tok);*/
    /*print_token(color(6)"EXPECTED_TOKEN"end_color, expected_tok);*/
    ASSERT_TYPES(tok, type, expected_type);
    printf("\n");
  }
  arena_free(&arena);
}

typedef struct {
  char *name;
  int z;
} Foo;

typedef struct {
  Foo tok;
  int y[128];
} Fuzz;

bool test_let_statement(Node statement, String expected_name) {
  // TODO: make this to give the token type in string, it now spits out what the
  // user typed `TokenType`
  if (statement.type != LET_STATEMENT) {
    printf(
        color(1) "ERROR:" end_color //
            color(4) "\n\tthe statement token is " color(
                1) "NOT" end_color
                   " a " end_color color(6) "`let statement` " end_color color(
                       7) "\n\tgot: `%s`\n" end_color,
        statement.token.literal.str);
    return false;
  }

  LetStatement let_stmt = *(LetStatement *)statement.data;
  if (!string_equals(let_stmt.name.value, expected_name)) {
    printf(
        color(1) "ERROR:" end_color //
            color(
                4) "\n\tthe *EXPECTED* let_stmt.name.value name and the *GOT* "
                   "one" color(1) "\n\tARENT THE SAME" end_color
                       color(6) "\n\texpected : `%s`" end_color color(
                           7) "\tgot:`%s`\n" end_color,
        expected_name.str, let_stmt.name.value.str);
    return false;
  }

  if (!string_equals(let_stmt.name.token.literal, expected_name)) {
    printf(color(1) "ERROR:" end_color //
               color(4) "\n\tthe *EXPECTED* let_stmt.name.token.literal name "
                        "and the *GOT* "
                        "one" color(1) "\n\tARENT THE SAME" end_color
                            color(6) "\n\texpected : `%s`" end_color color(
                                7) "\tgot:`%s`\n" end_color,
           expected_name.str, let_stmt.name.token.literal.str);
    return false;
  }
  return true;
}

bool test_return_statement(Node statement) {
  if (statement.type != RETURN_STATEMENT) {
    printf(
        color(1) "ERROR:" end_color //
            color(4) "\n\tthe statement token is " color(
                1) "NOT" end_color
                   " a " end_color color(6) "`let statement` " end_color color(
                       7) "\n\tgot: `%s`\n" end_color,
        statement.token.literal.str);
    return false;
  }
  return true;
}

void test_check_parser_errors(Parser *parser) {
  Error *errors = parser->errors;
  U64 err_len = len(errors);
  if (!err_len) {
    printf("There are not parsing errors!\n");
  } else {
    printf("There %zu parsing errors!\n", err_len);
    for (int i = 0; i < err_len; i++) {
      Error err = errors[i];
      printfln("%S", err.error);
    }
  }
}

TEST(test_parser_let_statement) {
  Arena arena = (Arena){.begin = NULL, .end = NULL};
  String input = arena_new_string(&arena, "let x = 5;\n"
                                          "let y = 10;\n"
                                          "let z = 34234234;\n");

  Lexer lexer = lexer_new_lexer(input);
  Parser parser = ast_new_parser(&arena, &lexer);
  Program program = ast_parse_program(&arena, &parser);
  (void)program;

  String expected_identifiers[] = {
      string("x"),
      string("y"),
      string("z"),
  };

  test_check_parser_errors(&parser);

  for (int i = 0; i < array_len(expected_identifiers); i++) {
    Node curr_statement = program.statements[i];
    failed = !test_let_statement(curr_statement, expected_identifiers[i]);
  }
  print_program(&program);
  arena_free(&arena);
}

#define end_program goto exit_program

int test_expressions() {
  Arena arena = (Arena){.begin = NULL, .end = NULL};
  String input = arena_new_string(&arena, "foobar;");
  Lexer lexer = lexer_new_lexer(input);
  Parser parser = ast_new_parser(&arena, &lexer);
  Program program = ast_parse_program(&arena, &parser);

  String expected_identifiers[] = {
      string("foobar"),
  };

  if (len(program.statements) != 1) {
    printf(LOG_ERROR "There are not 1 statements, only %zu\n",
           len(program.statements));
    end_program;
  }

  if (program.statements[0].type != EXPRESSION_STATEMENT) {
    printf(LOG_ERROR "The statement type is not a EXPRESSION_STATEMENT\n");
    end_program;
  }

  ExpressionStatement expr = *(ExpressionStatement *)program.statements[0].data;
  String identifier = expr.expression_value.token.literal;
  (void)identifier;

  if (expr.expression_value.token.type != IDENTIFIER) {
    printf(LOG_ERROR "The expression type is not a IDENTIFIER\n");
    end_program;
  }

  test_check_parser_errors(&parser);
  if (!string_equals(expr.expression_value.token.literal,
                     expected_identifiers[0])) {
    printfln(LOG_ERROR "The expression literal: `%S` is not: `%S`\n",
             expr.expression_value.token.literal, expected_identifiers[0]);
    arena_free(&arena);
  }
  print_program(&program);
exit_program:
  arena_free(&arena);
  failed = 0;
  return failed;
}
int main() {
  /*Arena arena = (Arena){.begin = NULL, .end = NULL};*/
  /*TokenType type = get_token_type("ILLEGAL");*/
  test_more_tokens();
  /*test_more_more_tokens();*/
  /*prefix_parse_fn fnptr = (prefix_parse_fn)get_ arena_free(&arena);*/
  return failed;
}
