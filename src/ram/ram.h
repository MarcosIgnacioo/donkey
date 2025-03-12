#ifndef _RAM_H
#define _RAM_H
#include "../arena_hash_table.c"

typedef struct {
  String key;
  bool is_occupied;
  union {
    int integer_value;
    String string_value;
  };
} KeyValueMemory;

typedef struct {
  String key;
  bool is_occupied;
  int value;
} KeyValueIntegers;

typedef struct {
  String key;
  bool is_occupied;
  String value;
} KeyValueStrings;

typedef enum {
  NIL_MEMORY,
  INTEGERS_MEMORY,
  STRINGS_MEMORY,
} MemoryType;

HashTable MEMORY[3] = {
    (HashTable){0},
    (HashTable){0},
    (HashTable){0},
};

void bind_string_value_to_variable_in_ram(byte *item, int hash, String value);
void bind_integer_value_to_variable_in_ram(byte *item, int hash, int value);
bool RAM_key_value_equals(void *this, void *that);
byte *get_item_in_hash_table(HashTable ht, String key);
KeyValueStrings *insert_string_to_memory(String key, String value);
KeyValueIntegers *insert_integer_to_memory(String key, int value);
String get_string_from_memory(String key);
int get_integer_from_memory(String key);

#endif // !_RAM_H
