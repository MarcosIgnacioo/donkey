
#ifndef _TEST_PARSER_H
#include "./test.h"

typedef struct {
  char *input;
  I64 expected;
} TestResultParser;

void test_parser() {
  Arena arena = (Arena){.begin = NULL, .end = NULL};
  char *input = "[12,3,4];";
  String expected = string("[12, 3, 4]");
  Lexer lexer = lexer_new_lexer(string(input));
  Parser parser = ast_new_parser(&arena, &lexer);
  Program program = ast_parse_program(&arena, &parser);
  ExpressionStatement expression_statement =
      program.statements[0].expression_statement;
  Expression *expression = expression_statement.expression_value;
  String string_literal = stringify_expression(&arena, NULL, expression);
  /*StringLiteral string_literal = expression->string_literal;*/
  if (!string_equals(string_literal, expected)) {
    printfln(LOG_ERROR "%S != %S\n", string_literal, expected);
  } else {
    printfln(LOG_SUCCESS "%S == %S\n", string_literal, expected);
  }
}

#endif /* ifndef _TEST_PARSER_H */
