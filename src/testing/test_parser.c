
#ifndef _TEST_PARSER_H
#include "../parser/donkey_hashmap.c"
#include "./test.h"

typedef struct {
  char *input;
  I64 expected;
} TestResultParser;

void test_parser() {
  Arena arena = (Arena){.begin = NULL, .end = NULL};
  // why the object keeps the } as a token and the parser thinks is consumable
  // or parseable
  /*char *input = "[1,1]";*/
  char *inputs[] = {"[1,1]", "{1:1}"};
  for (size_t i = 0; i < array_len(inputs); i += 1) {
    printf("------\n");
    Lexer lexer = lexer_new_lexer(string(inputs[i]));
    Parser parser = ast_new_parser(&arena, &lexer);
    Program program = ast_parse_program(&arena, &parser);
    for (size_t i = 0; i < len(program.statements); i++) {
      Node *node = &program.statements[i];
      Expression *exp = node->expression_statement.expression_value;
      String exp_str = stringify_expression(&arena, NULL, exp);
      print_parser_errors(parser);
      printfln("%S", exp_str);
    }
  }
  /*for (size_t i = 0; i < len(program.statements); i += 2) {*/
  /*  Node *left = &program.statements[i];*/
  /*  Node *right = &program.statements[i + 1];*/
  /*  Expression *key = left->expression_statement.expression_value;*/
  /*  Expression *value = right->expression_statement.expression_value;*/
  /*  donkey_hash_map_insert(&arena, &table, key, value);*/
  /*  Expression *needle = donkey_hash_map_get_item(&table, key);*/
  /*  String key_str = stringify_expression(&arena, NULL, key);*/
  /*  String value_str = stringify_expression(&arena, NULL, needle);*/
  /*  printfln("%S : %S", key_str, value_str);*/
  /*}*/
}

#endif /* ifndef _TEST_PARSER_H */
