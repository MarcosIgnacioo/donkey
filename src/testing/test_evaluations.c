#include "../array.c"
#include "../object/object.c"
#include "../ram/ram.h"
#include "test.h"
#include <stdio.h>

typedef struct {
  char *input;
  I64 expected;
} TestResultInteger;

void test_integer_evaluations() {
  TestResultInteger test_cases[] = {
      (TestResultInteger){.input = "-8", .expected = -8},
      (TestResultInteger){.input = "-5", .expected = -5},
      (TestResultInteger){.input = "8", .expected = 8},
      (TestResultInteger){.input = "5", .expected = 5},
      (TestResultInteger){.input = "-2", .expected = -2},
      (TestResultInteger){.input = "5", .expected = 5},
      (TestResultInteger){.input = "10", .expected = 10},
      (TestResultInteger){.input = "-5", .expected = -5},
      (TestResultInteger){.input = "-10", .expected = -10},
      (TestResultInteger){.input = "5 + 5 + 5 + 5 - 10", .expected = 10},
      (TestResultInteger){.input = "2 * 2 * 2 * 2 * 2", .expected = 32},
      (TestResultInteger){.input = "-50 + 100 + -50", .expected = 0},
      (TestResultInteger){.input = "5 * 2 + 10", .expected = 20},
      (TestResultInteger){.input = "5 + 2 * 10", .expected = 25},
      (TestResultInteger){.input = "20 + 2 * -10", .expected = 0},
      (TestResultInteger){.input = "50 / 2 * 2 + 10", .expected = 60},
      (TestResultInteger){.input = "2 * (5 + 10)", .expected = 30},
      (TestResultInteger){.input = "3 * 3 * 3 + 10", .expected = 37},
      (TestResultInteger){.input = "3 * (3 * 3) + 10", .expected = 37},
      (TestResultInteger){.input = "(5 + 10 * 2 + 15 / 3) * 2 + -10",
                          .expected = 50},
  };
  Object test_obj;
  bool pass = true;
  for (I64 i = 0; i < array_len(test_cases); i++) {
    TestResultInteger test = test_cases[i];
    test_obj = test_eval(test.input);
    if (!test_object_integer(test_obj, test.expected)) {
      printf("FAILED:%s\n", test.input);
      printf("expected:%lld\n", test.expected);
      printfln("got:%S\n", object_to_string(&arena, test_obj));
      pass = false;
    }
  }
  printfln("Last expression evaluated to: %S",
           object_to_string(&arena, test_obj));

  // TODO: Find a way to make the part of the function name not be hardcoded and
  //       just in a macro cause its better!! i hope
  if (pass) {
    printf(LOG_SUCCESS "ALL TEST PASSED AT: test_integer_evaluations() \n");
  } else {
    printf(LOG_ERROR "TEST FAILED       AT: test_integer_evaluations() \n");
  }
}

typedef struct {
  char *input;
  bool expected;
} TestResultBool;

void test_bool_evaluations() {
  TestResultBool test_cases[] = {
      (TestResultBool){.input = "(5 > 5 == true)", .expected = false},
      (TestResultBool){.input = "true", .expected = true},
      (TestResultBool){.input = "false", .expected = false},
      (TestResultBool){.input = "!false", .expected = true},
      (TestResultBool){.input = "!true", .expected = false},
      (TestResultBool){.input = "!!false", .expected = false},
      (TestResultBool){.input = "!!true", .expected = true},
  };
  Object test_obj;
  bool pass = true;
  for (I64 i = 0; i < array_len(test_cases); i++) {
    TestResultBool test = test_cases[i];
    test_obj = test_eval(test.input);
    if (!test_object_bool(test_obj, test.expected)) {
      pass = false;
    }
  }
  printfln("Last expression evaluated to: %S",
           object_to_string(&arena, test_obj));

  // TODO: Find a way to make the part of the function name not be hardcoded and
  //       just in a macro cause its better!! i hope
  if (pass) {
    printf(LOG_SUCCESS "ALL TEST PASSED AT: test_bool_evaluations() \n");
  } else {
    printf(LOG_ERROR "TEST FAILED       AT: test_bool_evaluations() \n");
  }
}

typedef struct {
  char *input;
  String expected;
} TestResultError;

bool test_object_error(Object testing, String expected) {
  if (testing.type != ERROR_OBJECT) {
    printf("\n%s!=OBJECT_ERROR\n", ObjectToString(testing.type));
    return false;
  }
  if (!string_equals(testing.error.value, expected)) {
    printf("\n%s!=%s\n", testing.error.value.str, expected.str);
    return false;
  }
  return true;
}

void test_error_handling() {
  TestResultError test_cases[] = {
      (TestResultError){
          //
          .input = "true + 5;",
          .expected = string("type mismatch: BOOLEAN + INTEGER")
          //
      },
      (TestResultError){
          //
          .input = "true + 5;",
          .expected = string("type mismatch: BOOLEAN + INTEGER")
          //
      },
      (TestResultError){
          .input = "5 + true; 5;",
          .expected = string("type mismatch: INTEGER + BOOLEAN"),
      },
      (TestResultError){
          .input = "-true",
          .expected = string("unknown operator: -BOOLEAN"),
      },
      (TestResultError){
          .input = "true + false;",
          .expected = string("unknown operator: BOOLEAN + BOOLEAN"),
      },
      (TestResultError){
          .input = "5; true + false; 5",
          .expected = string("unknown operator: BOOLEAN + BOOLEAN"),
      },
      (TestResultError){
          .input = "if (10 > 1) { true + false; }",
          .expected = string("unknown operator: BOOLEAN + BOOLEAN"),
      },
      (TestResultError){
          .input =
              "if (10 > 1) { if (10 > 1) { return true + false; } return 1; }",
          .expected = string("unknown operator: BOOLEAN + BOOLEAN")},
      (TestResultError){.input = "foobar",
                        .expected = string("identifier not found: foobar")}};

  Object test_obj;
  bool pass = true;
  for (I64 i = 0; i < array_len(test_cases); i++) {
    TestResultError test = test_cases[i];
    test_obj = test_eval(test.input);
    if (!test_object_error(test_obj, test.expected)) {
      pass = false;
    }
  }
  printfln("Last expression evaluated to: %S",
           object_to_string(&arena, test_obj));

  // TODO: Find a way to make the part of the function name not be hardcoded and
  //       just in a macro cause its better!! i hope
  if (pass) {
    printf(LOG_SUCCESS "ALL TEST PASSED AT: test_error_handling() \n");
  } else {
    printf(LOG_ERROR "TEST FAILED       AT: test_error_handling() \n");
  }
  printf("\n");
}

typedef struct {
  char *input;
  I64 expected;
} TestLetStatementsIdentifiers;

void test_let_statements_integers() {
  TestLetStatementsIdentifiers test_cases[] = {
      {.input = "let foo = 1; foo;", .expected = 1},
      {.input = "let a = 5; a;", .expected = 5},
      {.input = "let a = 5 * 5; a;", .expected = 25},
      {.input = "let a = 5; let b = a; b;", .expected = 5},
      {.input = "let a = 5; let b = a; let c = a + b + 5; c;", .expected = 15},
  };
  Object testing;
  bool pass = true;
  for (I64 i = 0; i < array_len(test_cases); i++) {
    TestLetStatementsIdentifiers test = test_cases[i];
    testing = test_eval(test.input);
    test_object_integer(testing, test.expected);
  }
  printfln("Last expression evaluated to: %S",
           object_to_string(&arena, testing));

  // TODO: Find a way to make the part of the function name not be hardcoded and
  //       just in a macro cause its better!! i hope
  if (pass) {
    printf(LOG_SUCCESS
           "ALL TEST PASSED AT: test_let_statements_and_identifiers() \n");
  } else {
    printf(LOG_ERROR
           "TEST FAILED       AT: test_let_statements_and_identifiers() \n");
  }
}

typedef struct {
  char *input;
} TestGeneric;

void test_repl() {
  env_init(&arena, &env);
  TestGeneric test_cases[] = {
      {.input = "let newAdder = fn(x) { return fn foo(y) { return x + y } }"},
      {.input = "fn foo(x){return x;}"},
      {.input = "foo(3);"},
  };
  String input = {0};
  Lexer lexer = (Lexer){0};
  Parser parser = {0};
  Program program = {0};
  String program_str;
  Object evaluation;
  for (I64 i = 0; i < array_len(test_cases); i++) {
    TestGeneric test = test_cases[i];
    input = string(test.input);
    lexer = lexer_new_lexer(input);
    parser = ast_new_parser(&arena, &lexer);
    program = ast_parse_program(&arena, &parser);
    evaluation = eval_evaluate_program(&arena, &env, program);
    program_str = object_to_string(&arena, evaluation);
    printfln(color(4) "DONKEY >> " end_color "%S", program_str);
    /*if (len(parser.errors)) {*/
    /*  print_parser_errors(parser);*/
    /*} else if (program_str.len) {*/
    /*}*/
  }
}

void test_generic() {
  env_init(&arena, &env);
  TestGeneric test_cases[] = {{.input = "let foo = 17;"
                                        "let newAdder = fn(x) {"
                                        "fn(y) { foo;x + y; };"
                                        "};"
                                        "let addTwo = newAdder(2);"
                                        "addTwo(2);"}};
  Object testing;
  for (I64 i = 0; i < array_len(test_cases); i++) {
    TestGeneric test = test_cases[i];
    testing = test_eval(test.input);
    printfln("%S", object_to_string(&arena, testing));
  }
  (void)testing;
  printf("\n");
}

typedef struct {
  char *input;
  I64 expected;
} TestFunctionApplication;
void test_function_application() {
  env_init(&arena, &env);
  TestFunctionApplication test_cases[] = {
      {.input = "let identity = fn(x) { x; }; identity(5);", .expected = 5},
      {.input = "let add = fn(x, y) { x + y; }; add(5 + 5, add(5, 5));",
       .expected = 20},
      {.input = "let identity = fn(x) { return x; }; identity(5);",
       .expected = 5},
      {.input = "let double = fn(x) { x * 2; }; double(5);", .expected = 10},
      {.input = "let add = fn(x, y) { x + y; }; add(5, 5);", .expected = 10},
      {.input = "fn(x) { x; }(5)", .expected = 5},
      {.input = "let foo = 17;"
                "let newAdder = fn(x) {"
                "fn(y) { foo;x + y; };"
                "};"
                "let addTwo = newAdder(2);"
                "addTwo(2);"
                "let foo = 23"
                "let bar = 26"
                "addTwo(13);"
                "addTwo(16);"
                "x;",
       .expected = -999}};
  Object test_obj;
  bool pass = true;
  for (I64 i = 0; i < array_len(test_cases); i++) {
    TestFunctionApplication test = test_cases[i];
    test_obj = test_eval(test.input);
    printfln("TEST ID:%d", i, test.input);
    printfln("%s", test.input);
    printfln("EVALUATED TO:");
    printfln("%S", object_to_string(&arena, test_obj));
    printf("\n");
    if (test.expected != -999 && !test_object_integer(test_obj, test.expected)) {
      printf("FAILED:%s\n", test.input);
      printf("expected:%lld\n", test.expected);
      printfln("got:%S\n", object_to_string(&arena, test_obj));
      pass = false;
    }
  }
  printfln("Last expression evaluated to: %S",
           object_to_string(&arena, test_obj));

  // TODO: Find a way to make the part of the function name not be hardcoded and
  //       just in a macro cause its better!! i hope
  if (pass) {
    printf(LOG_SUCCESS "ALL TEST PASSED AT: test_integer_evaluations() \n");
  } else {
    printf(LOG_ERROR "TEST FAILED       AT: test_integer_evaluations() \n");
  }
  printf("\n");
}

typedef struct {
  char *input;
  union {
    int value;
    void * not;
  };
} TestResultIfExpression;

void test_if_expressions_evaluations() {
  TestResultIfExpression test_cases[] = {
      (TestResultIfExpression){.input = "return 10;", .value = 10},
      (TestResultIfExpression){.input = "return 10; 9;", .value = 10},
      (TestResultIfExpression){.input = "return 2 * 5; 9;", .value = 10},
      (TestResultIfExpression){.input = "9; return 2 * 5; 9;", .value = 10},
      (TestResultIfExpression){
          .input =
              "if (10 > 1) { if (10 > 1) { 123; return 10; 456;} return 1; }",
          .value = 10},
      (TestResultIfExpression){.input = "if (true) { 10 }", .value = 10},
      (TestResultIfExpression){.input = "if (false) { 10 }", .not= nil},
      (TestResultIfExpression){.input = "if (1) { 10 }", .value = 10},
      (TestResultIfExpression){.input = "if (1 < 2) { 10 }", .value = 10},
      (TestResultIfExpression){.input = "if (1 > 2) { 10 }", .not= nil},
      (TestResultIfExpression){.input = "if (1 > 2) { 10 } else { 20 }",
                               .value = 20},
      (TestResultIfExpression){.input = "if (1 < 2) { 10 } else { 20 }",
                               .value = 10},
  };
  Object test_obj;
  bool pass = true;
  for (I64 i = 0; i < array_len(test_cases); i++) {
    TestResultIfExpression test = test_cases[i];
    test_obj = test_eval(test.input);
    if (test_obj.type == INTEGER_OBJECT) {
      if (!test_object_integer(test_obj, test.value)) {
        pass = false;
      }
    } else if (!test_object_null(test_obj)) {
      pass = false;
    }
  }
  printfln("Last expression evaluated to: %S",
           object_to_string(&arena, test_obj));

  if (pass) {
    printf(LOG_SUCCESS
           "ALL TEST PASSED AT: test_if_expressions_evaluations() \n");
  } else {
    printf(LOG_ERROR
           "TEST FAILED       AT: test_if_expressions_evaluations() \n");
  }
}

typedef struct {
  char *input;
  union {
    int value;
    void * not;
  };
} TestResultReturnExpressions;

void test_return_expressions_evaluations() {
  TestResultReturnExpressions test_cases[] = {
      (TestResultReturnExpressions){.input = "return 10;", .value = 10},
      (TestResultReturnExpressions){.input = "return 10; 9;", .value = 10},
      (TestResultReturnExpressions){.input = "return 2 * 5; 9;", .value = 10},
      (TestResultReturnExpressions){.input = "9; return 2 * 5; 9;",
                                    .value = 10},
      (TestResultReturnExpressions){
          .input =
              "if (10 > 1) { if (10 > 1) { 123; return 10; 456;} return 1; }",
          .value = 10},
  };
  Object test_obj;
  bool pass = true;
  for (I64 i = 0; i < array_len(test_cases); i++) {
    TestResultReturnExpressions test = test_cases[i];
    test_obj = test_eval(test.input);
    if (test_obj.type == INTEGER_OBJECT) {
      if (!test_object_integer(test_obj, test.value)) {
        pass = false;
      }
    } else if (!test_object_null(test_obj)) {
      pass = false;
    }
  }
  printfln("Last expression evaluated to: %S",
           object_to_string(&arena, test_obj));

  if (pass) {
    printf(LOG_SUCCESS
           "ALL TEST PASSED AT: test_return_expressions_evaluations() \n");
  } else {
    printf(LOG_ERROR
           "TEST FAILED       AT: test_return_expressions_evaluations() \n");
  }
}

Object test_eval(char *input) {
  Lexer lexer = lexer_new_lexer(string(input));
  Parser parser = ast_new_parser(&arena, &lexer);
  Program program = ast_parse_program(&arena, &parser);
  Object evaluated = eval_evaluate_program(&arena, &env, program);
  return evaluated;
}

// TODO : Implement here the number of the test with passin the i value
// in the loop
// optional find a way to show the whole test stateemnt maybe just passing the
// string or something like that girl
bool test_object_integer(Object testing, I64 expected) {
  if (testing.type != INTEGER_OBJECT) {
    printf("\n%s!=INTEGER_OBJECT\n", ObjectToString(testing.type));
    return false;
  }

  ObjectInteger testing_integer = testing.integer;

  if (testing_integer.value != expected) {
    printf("expected: %lld\ngot:\t  %lld\n", expected, testing_integer.value);
    return false;
  }

  return true;
}

bool test_object_bool(Object testing, bool expected) {
  if (testing.type != BOOLEAN_OBJECT) {
    printf("\n%s!=BOOLEAN_OBJECT\n", ObjectToString(testing.type));
    return false;
  }

  ObjectBoolean testing_bool = testing.boolean;

  if (testing_bool.value != expected) {
    printfln("expected: %b\ngot:\t  %b", expected, testing_bool.value);
    return false;
  }

  return true;
}

bool test_object_string(Object testing, String expected) {
  if (testing.type != STRING_OBJECT) {
    printf("\n%s!=STRING_OBJECT\n", ObjectToString(testing.type));
    return false;
  }

  ObjectString testing_string = testing.string;

  if (!string_equals(testing_string.value, expected)) {
    printfln("expected: %S\ngot:\t  %S", expected, testing_string.value);
    return false;
  }

  return true;
}

bool test_object_null(Object testing) {
  if (testing.type != NIL_OBJECT) {
    printf("\n%s!=NIL_OBJECT\n", ObjectToString(testing.type));
    return false;
  }
  return true;
}
