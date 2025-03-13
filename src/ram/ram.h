#ifndef _RAM_H
#define _RAM_H
#include "../arena_hash_table.c"
#include "../object/object.h"

typedef struct {
  String key;
  bool is_occupied;
  Object value;
} KeyValueMemory;

HashTable MEMORY = {0};

bool ram_key_value_equals(void *this, void *that);
void ram_insert_object(Arena *arena, String key, Object value);
Object ram_get_object(Arena *arena, String key);
#endif // !_RAM_H
