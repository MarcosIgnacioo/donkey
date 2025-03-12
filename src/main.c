#include "./ram/ram.c"
#include <stdbool.h>
#include <stdio.h>

int main() {
  Arena arena = {0};
  hash_table_alloc(&arena, &MEMORY[INTEGERS_MEMORY], KeyValueIntegers,
                   &RAM_key_value_equals);
  hash_table_alloc(&arena, &MEMORY[STRINGS_MEMORY], KeyValueStrings,
                   &RAM_key_value_equals);
  insert_string_to_memory(string("uwu"), string("owo"));
  insert_string_to_memory(string("iwi"), string("pipi"));
  insert_string_to_memory(string("ovo"), string("poop"));
  insert_string_to_memory(string("uvu"), string("asdkf"));
  insert_string_to_memory(string("ewe"), string("caca"));
  String key1 = string("uvu");
  String val1 = get_string_from_memory(key1);
  printfln("[%S]:[%S]", key1, val1);
  /*HashTable ht = {0};*/
  /*hash_table_alloc(&arena, &ht, KeyValueIntegers, &RAM_key_value_equals);*/
  /*String key1 = string("hello");*/
  /*String key2 = string("wtf");*/
  /*insert_string_to_memory(ht, key2, string("poop"));*/
  /*String val2 = get_string_from_memory(ht, key2);*/
  /*printfln("[%S]:[%S]", key2, val2);*/
  return 0;
}
