
#ifndef _TEST_PARSER_H
#include "./test.h"

typedef struct {
  char *input;
  I64 expected;
} TestResultParser;

void test_parser() {
  Arena arena = (Arena){.begin = NULL, .end = NULL};
  char *input = "\"hello world\";";
  String expected = string("hello world");
  Lexer lexer = lexer_new_lexer(string(input));
  Parser parser = ast_new_parser(&arena, &lexer);
  Program program = ast_parse_program(&arena, &parser);
  ExpressionStatement expression_statement =
      program.statements[0].expression_statement;
  Expression *expression = expression_statement.expression_value;
  StringLiteral string_literal = expression->string_literal;
  if (!string_equals(string_literal.value, expected)) {
    printfln(LOG_ERROR "%S != %S\n", string_literal.value, expected);
  } else {
    printfln(LOG_SUCCESS "%S == %S\n", string_literal.value,
             expected);
  }
}

#endif /* ifndef _TEST_PARSER_H */
