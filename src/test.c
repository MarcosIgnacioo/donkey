#include "./parser/ast.c"
#include "array.c"
#include "lexer.c"
#include "repl.c"
#include "token.c"
#include <assert.h>
#include <stdio.h>

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
    printf(LOG_SUCCESS "There are not parsing errors!\n");
  } else {
    printf(LOG_ERROR "There %zu parsing errors!\n", err_len);
    for (int i = 0; i < err_len; i++) {
      Error err = errors[i];
      printfln(LOG_ERROR "%S", err.error);
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
  print_program(&arena, &program);
  arena_free(&arena);
}

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
  Token identifier_exp = *(Token *)expr.expression_value.exp_bytes;

  if (expr.expression_value.type != IDENTIFIER_EXP) {
    printf(LOG_ERROR "The expression type is not a IDENTIFIER_EXP\n");
    end_program;
  }

  if (identifier_exp.type != IDENTIFIER) {
    printf(LOG_ERROR "The token type is not a IDENTIFIER\n");
    end_program;
  }

  test_check_parser_errors(&parser);
  if (!string_equals(identifier_exp.literal, expected_identifiers[0])) {
    printfln(LOG_ERROR "The expression literal: `%S` is not: `%S`\n",
             identifier_exp.literal, expected_identifiers[0]);
    arena_free(&arena);
  }
  print_program(&arena, &program);
  end_program;
exit_program:
  arena_free(&arena);
  failed = 0;
  return failed;
}

#define test_n_of_statements(N)                                                \
  if (len(program.statements) != N) {                                          \
    printf(LOG_ERROR "There are not %d statements, only %zu\n", N,             \
           len(program.statements));                                           \
  }
#define test_type(THIS, TYPE)                                                  \
  if ((THIS).type != TYPE) {                                                   \
    printf(LOG_ERROR "%s type is not a %s\n", #THIS, #TYPE);                   \
  }

// TOFIX: STOP with these
// with these what
// ahhh the exit_program tag yeah kinda
bool test_integer_literal(Arena *arena, Expression exp, I64 value) {
  test_type(exp, INTEGER_LIT_EXP);
  IntLiteral int_exp = cast(exp.exp_bytes, IntLiteral);
  if (int_exp.value != value) {
    printf(LOG_ERROR "%lld != %lld\n", int_exp.value, value);
    return false;
  }
  String lit = arena_string_fmt(arena, "%d", value);
  Token tmp_tok = NEW_TOKEN(INT, lit);
  if (!token_equals(int_exp.token, tmp_tok)) {
    printf(LOG_ERROR "tokens are not the same\n");
    print_token("int exp checking", int_exp.token);
    print_token("tmp token created", tmp_tok);
    return false;
  }
  return true;
}

int test_infix_expressions() {
  Arena arena = (Arena){.begin = NULL, .end = NULL};

  typedef struct {
    String input;
    String operator;
    I64 left_value;
    I64 right_value;
  } infix_test;

  infix_test expected_identifiers[] = {//
                                       (infix_test){
                                           .input = string("a + b"),
                                           .operator= string("+"),
                                           .left_value = 1,
                                           .right_value = 2,
                                       }};

  for (int i = 0; i < array_len(expected_identifiers); i++) {
    infix_test test = expected_identifiers[i];
    String input = test.input;
    Lexer lexer = lexer_new_lexer(input);
    Parser parser = ast_new_parser(&arena, &lexer);
    Program program = ast_parse_program(&arena, &parser);
    print_parser_errors(parser);
    test_n_of_statements(1);
    test_type(program.statements[0], EXPRESSION_STATEMENT);
    ExpressionStatement expr =
        cast_statement(program.statements[0], ExpressionStatement);
    test_type(expr.expression_value, INFIX_EXP);
    InfixExpression prefix_exp =
        cast(expr.expression_value.exp_bytes, InfixExpression);
    if (!string_equals(prefix_exp.operator, test.operator)) {
      printf(LOG_ERROR "prefix_exp.operator %s != test.operator %s\n",
             prefix_exp.operator.str, test.operator.str);
    }
    print_program(&arena, &program);
    test_integer_literal(&arena, prefix_exp.right, test.right_value);
    test_integer_literal(&arena, prefix_exp.left, test.left_value);
  }

  end_program;
exit_program:
  arena_free(&arena);
  failed = 0;
  return failed;
}

int test_infix_expressions_harder() {
  Arena arena = (Arena){.begin = NULL, .end = NULL};

  typedef struct {
    String input;
    String output;
  } infix_test;

  infix_test expected_identifiers[] = {
      //
      (infix_test)                                                      //
      {.input = string("a + b + c"), .output = string("((a + b) + c)")} //
  };

  for (int i = 0; i < array_len(expected_identifiers); i++) {
    infix_test test = expected_identifiers[i];
    String input = test.input;
    Lexer lexer = lexer_new_lexer(input);
    Parser parser = ast_new_parser(&arena, &lexer);
    Program program = ast_parse_program(&arena, &parser);
    print_parser_errors(parser);
    test_n_of_statements(1);
    test_type(program.statements[0], EXPRESSION_STATEMENT);
    ExpressionStatement expr =
        cast_statement(program.statements[0], ExpressionStatement);
    test_type(expr.expression_value, INFIX_EXP);
    InfixExpression prefix_exp =
        cast(expr.expression_value.exp_bytes, InfixExpression);
    (void)prefix_exp;
    String program_str = stringify_program(&arena, &program);
    if (!string_equals(program_str, test.output)) {
      printf(LOG_ERROR "\n\t%s != %s\n", program_str.str, test.output.str);
    }
    printf("\n");
    print_program(&arena, &program);
  }

  end_program;
exit_program:
  arena_free(&arena);
  failed = 0;
  return failed;
}

int test_prefix_expressions() {
  Arena arena = (Arena){.begin = NULL, .end = NULL};

  typedef struct {
    String input;
    String operator;
    I64 value;
  } prefix_test;

  prefix_test expected_identifiers[] = {(prefix_test){
                                            .input = string("!5"),
                                            .operator= string("!"),
                                            .value = 5,
                                        },
                                        (prefix_test){
                                            .input = string("-15"),
                                            .operator= string("-"),
                                            .value = 15,
                                        }};

  for (int i = 0; i < array_len(expected_identifiers); i++) {
    prefix_test test = expected_identifiers[i];
    String input = test.input;
    Lexer lexer = lexer_new_lexer(input);
    Parser parser = ast_new_parser(&arena, &lexer);
    Program program = ast_parse_program(&arena, &parser);
    print_parser_errors(parser);
    test_n_of_statements(1);
    test_type(program.statements[0], EXPRESSION_STATEMENT);
    ExpressionStatement expr =
        cast_statement(program.statements[0], ExpressionStatement);
    test_type(expr.expression_value, PREFIX_EXP);
    PrefixExpression prefix_exp =
        cast(expr.expression_value.exp_bytes, PrefixExpression);

    if (!string_equals(prefix_exp.operator, test.operator)) {
      printf(LOG_ERROR "prefix_exp.operator %s != test.operator %s\n",
             prefix_exp.operator.str, test.operator.str);
    }

    print_program(&arena, &program);
    test_integer_literal(&arena, prefix_exp.right, test.value);

    /*if (!string_equals(prefix_exp.input, test.input)) {*/
    /*  printf(LOG_ERROR "prefix_exp.input %s != test.input %s\n",*/
    /*         prefix_exp.input.str, test.input.str);*/
    /*}*/
    /**/
    /*if (prefix_exp.value != test.value) {*/
    /*  printf(LOG_ERROR "prefix_exp.value %s != test.value %s\n",*/
    /*         prefix_exp.value.str, test.value.str);*/
    /*}*/
  }

  end_program;
exit_program:
  arena_free(&arena);
  failed = 0;
  return failed;
}

int test_expressions_integer_literals() {
  Arena arena = (Arena){.begin = NULL, .end = NULL};
  String input = arena_new_string(&arena, "123;");
  Lexer lexer = lexer_new_lexer(input);
  Parser parser = ast_new_parser(&arena, &lexer);
  Program program = ast_parse_program(&arena, &parser);

  Token expected_tokens[] = {
      NEW_TOKEN(INT, string("123")),
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

  ExpressionStatement expr_statement =
      *(ExpressionStatement *)program.statements[0].data;
  Expression exp = expr_statement.expression_value;
  IntLiteral integer_exp = *(IntLiteral *)exp.exp_bytes;

  if (exp.type != INTEGER_LIT_EXP) {
    printf(LOG_ERROR "The exp bytes type is not a INTEGER_LIT_EXP\n");
    end_program;
  }

  // find a way to make this automatically with a macro i guess XD because
  // rewritin it sucks
  for (int i = 0; i < array_len(expected_tokens); i++) {
    Token expected_tok = expected_tokens[i];
    if (!string_equals(integer_exp.token.literal, expected_tok.literal)) {
      printf(LOG_ERROR "expected : %s != got : %s", expected_tok.literal.str,
             integer_exp.token.literal.str);
      printf(LOG_ERROR "integer_exp.token.literal != expected_tok.literal");
      end_program;
    }
    if (expected_tok.type != integer_exp.token.type) {
      printf(LOG_ERROR "expected : %d != got : %d", expected_tok.type,
             integer_exp.token.type);
      end_program;
    }
  }

  if (integer_exp.value != 123) {
    printf(LOG_ERROR "expected : %d != got : %lld", 123, integer_exp.value);
    end_program;
  }

  print_program(&arena, &program);
  end_program;
exit_program:
  arena_free(&arena);
  failed = 0;
  return failed;
}

int test_expressions_beta() {
  Arena arena = (Arena){.begin = NULL, .end = NULL};
  String input = arena_new_string(&arena, "let x = foobar;");
  Lexer lexer = lexer_new_lexer(input);
  Parser parser = ast_new_parser(&arena, &lexer);
  Program program = ast_parse_program(&arena, &parser);
  test_check_parser_errors(&parser);
  print_program(&arena, &program);
  end_program;
exit_program:
  arena_free(&arena);
  failed = 0;
  return failed;
}

int main() {
  /*Arena arena = (Arena){.begin = NULL, .end = NULL};*/
  /*TokenType type = get_token_type("ILLEGAL");*/
  /*test_more_more_tokens();*/
  /*test_more_more_tokens();*/
  /*prefix_parse_fn fnptr = (prefix_parse_fn)get_ arena_free(&arena);*/
  /*test_expressions();*/
  /*test_expressions_integer_literals();*/
  /*test_prefix_expressions();*/
  test_infix_expressions_harder();
  return failed;
}
