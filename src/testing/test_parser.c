
#ifndef _TEST_PARSER_H
#include "./test.h"

typedef struct {
  char *input;
  I64 expected;
} TestResultParser;

void test_parser() {
  Arena arena = (Arena){.begin = NULL, .end = NULL};
  char *input = "[true];";
  String expected = string("[true]");
  (void)input;
  (void)expected;
  Lexer lexer = lexer_new_lexer(string(input));
  Parser parser = ast_new_parser(&arena, &lexer);
  Program program = ast_parse_program(&arena, &parser);
  print_parser_errors(parser);
  Enviroment env = {0};
  env_init(&arena, &env);
  Object evaluated = eval_evaluate_program(&arena, &env, program);
  (void)evaluated;
  /*ExpressionStatement expression_statement =*/
  /*    program.statements[0].expression_statement;*/
  /*Expression *expression = expression_statement.expression_value;*/
  /*String string_literal = stringify_expression(&arena, NULL, expression);*/
  /*StringLiteral string_literal = expression->string_literal;*/
  /*if (!string_equals(string_literal, expected)) {*/
  /*  printfln(LOG_ERROR "%S != %S\n", string_literal, expected);*/
  /*} else {*/
  /*  printfln(LOG_SUCCESS "%S == %S\n", string_literal, expected);*/
  /*}*/
}

#endif /* ifndef _TEST_PARSER_H */
