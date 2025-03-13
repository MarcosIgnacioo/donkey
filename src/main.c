#include "./lexer.c"
#include "./object/object.c"
#include "./parser/ast.c"
#include "./ram/ram.h"
#include <stdbool.h>
#include <stdio.h>
#include "./repl.c"
Arena arena = {0};

Object test_eval(char *input) {
  Lexer lexer = lexer_new_lexer(string(input));
  Parser parser = ast_new_parser(&arena, &lexer);
  Program program = ast_parse_program(&arena, &parser);
  Object evaluated = eval_evaluate_program(&arena, program);
  return evaluated;
}

int main() {
  donkey_repl(&arena);
  /*HashTable ht = {0};*/
  /*hash_table_alloc(&arena, &ht, KeyValueIntegers, &RAM_key_value_equals);*/
  /*String key1 = string("hello");*/
  /*String key2 = string("wtf");*/
  /*insert_string_to_memory(ht, key2, string("poop"));*/
  /*String val2 = get_string_from_memory(ht, key2);*/
  /*printfln("[%S]:[%S]", key2, val2);*/
  return 0;
}
