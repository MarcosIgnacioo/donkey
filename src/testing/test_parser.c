#ifndef _TEST_PARSER_H
#include "./test.h"

typedef struct {
  char *input;
  I64 expected;
} TestResultParser;
#include "../object/object_hashmap.c"

void test_parser() {
  Arena arena = (Arena){.begin = NULL, .end = NULL};
  // why the object keeps the } as a token and the parser thinks is consumable
  // or parseable
  /*char *input = "[1,1]";*/
  /*char *inputs[] = {"asdf:3"};*/
  char *inputs[] = {"let x = {1:2,3:4}; x[true]"};
    Enviroment env = {0};
  for (size_t i = 0; i < array_len(inputs); i += 1) {
    Lexer lexer = lexer_new_lexer(string(inputs[i]));
    Parser parser = ast_new_parser(&arena, &lexer);
    Program program = ast_parse_program(&arena, &parser);
    print_parser_errors(parser);
    Object obj = eval_evaluate_program(&arena, &env, program);
    String obj_str = object_to_string(&arena, obj);
    printfln("%S", obj_str);
    printf("\n");
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
