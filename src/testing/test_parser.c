#ifndef _TEST_PARSER_H
#include "../parser/donkey_hashmap.c"
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
  char *inputs[] = {"{\"hola\":1 + 123 /3, \"uwu\":\"owo\", 1:3213, true:false, true : 1}", "[1,1]"};
  for (size_t i = 0; i < 1; i += 1) {
    printf("------\n");
    Lexer lexer = lexer_new_lexer(string(inputs[i]));
    Parser parser = ast_new_parser(&arena, &lexer);
    Program program = ast_parse_program(&arena, &parser);
    Enviroment env = {0};
    Object obj = eval_evaluate_program(&arena, &env, program);
    String obj_str = object_to_string(&arena, obj);
    printfln("%S", obj_str);
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
