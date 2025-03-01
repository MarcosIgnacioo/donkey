#include "./test.h"
#include <stdarg.h>
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
    failed =
        !test_let_statement(curr_statement, expected_identifiers[i], "foo");
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

bool test_identifier(Expression expression, String value) {
  if (expression.type != IDENTIFIER_EXP) {
    print_error("Expression type is not IDENTIFIER\n");
    return false;
  }
  Identifier exp_ident = cast(expression.exp_bytes, Identifier);
  if (!string_equals(exp_ident.value, value)) {
    print_error(
        "Identifiers values are not the same \n\tgot: %s\n\texpected: %s\n",
        exp_ident.value.str, value.str);
    return false;
  }
  if (!string_equals(exp_ident.token.literal, value)) {
    print_error("Token literal in expression identifier and value are not the "
                "same \n\tgot: %s\n\texpected: %s\n",
                exp_ident.value.str, value.str);
    return false;
  }
  return true;
}
// TOFIX: STOP with these
// with these what
// ahhh the exit_program tag yeah kinda
bool test_integer_literal(Expression exp, I64 value) {
  test_type(exp, INTEGER_LIT_EXP);
  IntLiteral int_exp = cast(exp.exp_bytes, IntLiteral);
  if (int_exp.value != value) {
    printf(LOG_ERROR "%lld != %lld\n", int_exp.value, value);
    return false;
  }
  // sprintf is easier bro
  char buf[1024];
  /*String lit = arena_string_fmt(arena, "%d", value);*/
  sprintf(buf, "%lld", value);
  String lit = string(buf);
  Token tmp_tok = NEW_TOKEN(INT, lit);
  if (!token_equals(int_exp.token, tmp_tok)) {
    printf(LOG_ERROR "tokens are not the same\n");
    print_token("int exp checking", int_exp.token);
    print_token("tmp token created", tmp_tok);
    return false;
  }
  return true;
}

bool test_bool_literal(Expression exp, bool value) {
  test_type(exp, BOOLEAN_EXP);
  Boolean bool_exp = cast(exp.exp_bytes, Boolean);
  if (bool_exp.value != value) {
    printf(LOG_ERROR "%d != %d\n", bool_exp.value, value);
    return false;
  }
  // sprintf is easier bro
  char buf[1024];
  /*String lit = arena_string_fmt(arena, "%d", value);*/
  if (value) {
    sprintf(buf, "%s", "true");
  } else {
    sprintf(buf, "%s", "false");
  }
  String lit = string(buf);
  TokenType type = (value) ? TRUE : FALSE;
  Token tmp_tok = NEW_TOKEN(type, lit);
  if (!token_equals(bool_exp.token, tmp_tok)) {
    printf(LOG_ERROR "Tokens are not the same\n");
    print_token(LOG_INFO "bool exp checking", bool_exp.token);
    print_token(LOG_INFO "tmp token created", tmp_tok);
    return false;
  }

  return true;
}

typedef struct {
  String expected_name;
  void *expected_value;
} TestIdentifierARGS;

#define test_statement(STMNT, ...) _test_statement(STMNT, 1, __VA_ARGS__)

bool _test_statement(Node statement, int n, ...) {
  switch (statement.type) {
  case LET_STATEMENT:
    //
    {
      String expected_name;
      void *expected_value;
      n = 2;

      va_list args;
      va_start(args, n);
      expected_name = va_arg(args, String);
      expected_value = va_arg(args, void *);
      va_end(args);

      return test_let_statement(statement, expected_name, expected_value);
      break;
    }
  case RETURN_STATEMENT:
    //
    {
      // refactor this to use a macro were i do this
      // get_args_from_list(expected_name,expected_value);
      // using a helper macro and x macro will do the job i think
      // and typeof but that one aint portable i think
      void *expected_value;
      n = 1;

      va_list args;
      va_start(args, n);
      expected_value = va_arg(args, void *);
      va_end(args);

      return test_return_statement(statement, expected_value);
      break;
    }
  default:
    //
    {
      void *expected_value;
      n = 1;

      va_list args;
      va_start(args, n);
      expected_value = va_arg(args, void *);
      va_end(args);
      ExpressionStatement exp_stmnt = cast(statement.data, ExpressionStatement);
      Expression exp = exp_stmnt.expression_value;
      return test_literal_expression(exp, expected_value);
      break;
    }
  }
}

bool test_literal_expression(Expression exp, void *value) {
  switch (exp.type) {
  case IDENTIFIER_EXP:
    //
    {
      return test_identifier(exp, cast(value, String));
      break;
    }
  case INTEGER_LIT_EXP:
    //
    {
      return test_integer_literal(exp, cast(value, I64));
      break;
    }
  case BOOLEAN_EXP:
    //
    {
      return test_bool_literal(exp, cast(value, bool));
      break;
    }
  default:
    //
    {
      print_error("There is no helper function to this expression type\nIm so "
                  "sorry for that\n");
      return false;
      break;
    }
  }
}

bool test_infix_expression(Expression expression, void *left,
                           const char *operator, void * right) {
  if (expression.type != INFIX_EXP) {
    print_error("Expression type is not INFIX_EXP\n");
  }
  InfixExpression infix_exp = cast(expression.exp_bytes, InfixExpression);

  bool left_test = test_literal_expression(infix_exp.left, left);

  bool right_test = test_literal_expression(infix_exp.right, right);

  bool operator_test;

  if (c_string_equals(infix_exp.operator.str, operator)) {
    operator_test = false;
  } else {
    operator_test = true;
  }

  return !left_test && !right_test && !operator_test;
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
    test_integer_literal(prefix_exp.right, test.right_value);
    test_integer_literal(prefix_exp.left, test.left_value);
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
      {//
       .input = string("let x = 1 * 2 * 3 * 4 * 5"),
       .output = string("let x = ((((1 * 2) * 3) * 4) * 5)")},
      //
      {.input = string("x * y / 2 + 3 * 8 - 123"),
       .output = string("((((x * y) / 2) + (3 * 8)) - 123)")},
      //
      {.input = string("true == false"), .output = string("(true == false)")},
      {.input = string("let fii = asdf + 23"), .output = string("IGNORE")},
  };

  for (int i = 0; i < array_len(expected_identifiers); i++) {
    failed = 0;
    infix_test test = expected_identifiers[i];
    String input = test.input;
    Lexer lexer = lexer_new_lexer(input);
    Parser parser = ast_new_parser(&arena, &lexer);
    Program program = ast_parse_program(&arena, &parser);
    /*test_type(program.statements[0], EXPRESSION_STATEMENT);*/
    String program_str = stringify_program(&arena, &program);
    if (!string_equals(program_str, test.output) &&
        !string_equals(test.output, string("IGNORE"))) {
      printf(color(1) "[FAILED]" end_color "\n");
      failed = 1;
      printf(LOG_ERROR "\n%s \n" color(3) "*IS NOT EQUAL TO*:" end_color "\n%s",
             program_str.str, test.output.str);
      printf("\n\t");
    } else {
      printf(color(2) "[SUCCESS]" end_color "\n");
    }
    printf(color(4) "[INPUT]\n" end_color);
    printf("%s\n", input.str);
    printfln("S: %S", program_str);
    printf("s: %s\n", program_str.str);
    print_program(&arena, &program);
    print_parser_errors(parser);
    printf("\n");
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
    test_integer_literal(prefix_exp.right, test.value);

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

bool test_let_statement(Node statement, String expected_name,
                        void *expected_value) {
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

  return test_literal_expression(let_stmt.expression_value, expected_value);
}

bool test_return_statement(Node statement, void *expected_value) {
  ReturnStatement ret_stmt = *(ReturnStatement *)statement.data;
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
  return test_literal_expression(ret_stmt.expression_value, expected_value);
}

bool generic_test_infix_expression( //
    Expression expression,          //
    void *left,                     //
    const char *operator,           //
    void * right)                   //
{
  if (expression.type != INFIX_EXP) {
    print_error("Expression type is not INFIX_EXP\n");
  }
  InfixExpression infix_exp = cast(expression.exp_bytes, InfixExpression);

  bool left_test = test_literal_expression(infix_exp.left, left);

  bool right_test = test_literal_expression(infix_exp.right, right);

  bool operator_test;

  if (!c_string_equals(infix_exp.operator.str, operator)) {
    operator_test = false;
  } else {
    operator_test = true;
  }

  return left_test && right_test && operator_test;
}

int test_booleans() {
  Arena arena = (Arena){.begin = NULL, .end = NULL};
  bool t = true;
  bool f = false;

  typedef struct {
    String input;
    void *left_value;
    String operator;
    void *right_value;
  } Test;

  Test expected_identifiers[] = {
      {string("true == true"), &t, string("=="), &t},
      {string("true != false"), &t, string("!="), &f},
      {string("false == false"), &f, string("=="), &f},
  };

  for (int i = 0; i < array_len(expected_identifiers); i++) {
    failed = 0;
    Test test = expected_identifiers[i];
    String input = test.input;
    Lexer lexer = lexer_new_lexer(input);
    Parser parser = ast_new_parser(&arena, &lexer);
    Program program = ast_parse_program(&arena, &parser);
    print_parser_errors(parser);
    ExpressionStatement exp_stmnt =
        cast(program.statements[0].data, ExpressionStatement);
    failed = !generic_test_infix_expression(exp_stmnt.expression_value,
                                            test.left_value, test.operator.str,
                                            test.right_value);
    if (!failed) {
      printf(LOG_SUCCESS "There are not parsing errors!\n");
    }
  }

  arena_free(&arena);
  return failed;
}

int main() {
  test_infix_expressions_harder();
  return 0;
}
