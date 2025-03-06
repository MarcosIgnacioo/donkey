#include "../array.c"
#include "../object/object.c"
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
  };
  Object test_obj;
  bool pass = true;
  for (I64 i = 0; i < array_len(test_cases); i++) {
    TestResultInteger test = test_cases[i];
    test_obj = test_eval(test.input);
    if (!test_object_integer(test_obj, test.expected)) {
      pass = false;
    }
  }
  printfln("Last expression evaluated to: %S", object_to_string(&arena, test_obj));

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
  printfln("Last expression evaluated to: %S", object_to_string(&arena, test_obj));

  // TODO: Find a way to make the part of the function name not be hardcoded and
  //       just in a macro cause its better!! i hope
  if (pass) {
    printf(LOG_SUCCESS "ALL TEST PASSED AT: test_bool_evaluations() \n");
  } else {
    printf(LOG_ERROR "TEST FAILED       AT: test_bool_evaluations() \n");
  }
}

Object test_eval(char *input) {
  Lexer lexer = lexer_new_lexer(string(input));
  Parser parser = ast_new_parser(&arena, &lexer);
  Program program = ast_parse_program(&arena, &parser);
  Object evaluated = eval_evaluate_program(&arena, program);
  return evaluated;
}

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
