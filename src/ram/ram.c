#include "./ram.h"

bool ram_key_value_equals(void *this, void *that) {
  String this_str = (*(KeyValueMemory *)this).key;
  String that_str = *(String *)that;
  return string_equals(this_str, that_str);
}

void ram_insert_object(Arena *arena, String key, Object value) {
  if (!MEMORY.items) {
    hash_table_alloc(arena, &MEMORY, KeyValueMemory, &ram_key_value_equals);
  }

  if (MEMORY.len >= MEMORY.capacity) {
    MEMORY.items = realloc(MEMORY.items, MEMORY.capacity * 2);
    MEMORY.capacity = MEMORY.capacity * 2;
  }

  U64 hash = get_hash(key) % (MEMORY).capacity;
  KeyValueMemory *items = (MEMORY).items;
  KeyValueMemory *curr_item = &items[hash];

  while (curr_item->is_occupied && !(MEMORY).are_keys_equals(&curr_item, &key) &&
         hash < (MEMORY).capacity) {
    hash++;
    curr_item = &items[hash];
  }

  if (curr_item->is_occupied && !((MEMORY).are_keys_equals(&curr_item, &key))) {
    printf("OUT OF MEMORY IN DONKEYLANG\n");
    return;
  }

  if (!curr_item->is_occupied) {
    MEMORY.len++;
  }

  curr_item->key = key;
  curr_item->is_occupied = true;
  curr_item->value = value;
}

Object ram_get_object(String key) {
  if (!MEMORY.items) {
    return DONKEY_PANIC_OBJECT;
  }

  U64 hash = get_hash(key) % (MEMORY).capacity;
  KeyValueMemory *items = (MEMORY).items;
  KeyValueMemory *curr_item = &items[hash];

  while (curr_item->is_occupied && !(MEMORY).are_keys_equals(curr_item, &key) &&
         hash < (MEMORY).capacity) {
    hash++;
    curr_item = &items[hash];
  }

  if (curr_item->is_occupied && !((MEMORY).are_keys_equals(curr_item, &key))) {
    return DONKEY_PANIC_OBJECT;
  }

  return curr_item->value;
}
