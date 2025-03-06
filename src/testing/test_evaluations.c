#include "../array.c"
#include "../object/object.c"
#include "test.h"
#include <stdio.h>

typedef struct {
  char *input;
  I64 expected;
} TestResult;

void test_integer_evaluations() {
  TestResult test_cases[] = {
      (TestResult){.input = "8", .expected = 8},
      (TestResult){.input = "5", .expected = 5},
  };
  Object test_obj;
  bool pass = true;
  for (I64 i = 0; i < array_len(test_cases); i++) {
    TestResult test = test_cases[i];
    test_obj = test_eval(test.input);
    if (!test_object_integer(test_obj, test.expected)) {
      pass = false;
    }
  }
  printfln("%S", object_to_string(&arena, test_obj));

  // TODO: Find a way to make the part of the function name not be hardcoded and
  //       just in a macro cause its better!! i hope
  if (pass) {
    printf(LOG_SUCCESS "ALL TEST PASSED AT: test_integer_evaluations() \n");
  } else {
    printf(LOG_ERROR "TEST FAILED       AT: test_integer_evaluations() \n");
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
