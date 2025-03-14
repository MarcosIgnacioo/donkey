#include "./ram.h"
#include "../object/object.h"

void env_init(Arena *arena, Enviroment *env) {
  /*arena_reset(arena);*/
  hash_table_alloc(arena, &env->memory, KeyValueMemory, &env_key_value_equals);
}

bool env_key_value_equals(void *this, void *that) {
  String this_str = (*(KeyValueMemory *)this).key;
  String that_str = *(String *)that;
  return string_equals(this_str, that_str);
}

void env_insert_object(Arena *arena, Enviroment *env, String key,
                       Object value) {
  if (!env->memory.items) {
    hash_table_alloc(arena, &env->memory, KeyValueMemory, &env_key_value_equals);
  }

  if (env->memory.len >= env->memory.capacity) {
    env->memory.items = realloc(env->memory.items, env->memory.capacity * 2);
    env->memory.capacity = env->memory.capacity * 2;
  }

  U64 hash = get_hash(key) % (env->memory).capacity;
  KeyValueMemory *items = (env->memory).items;
  KeyValueMemory *curr_item = &items[hash];

  while (curr_item->is_occupied &&
         !(env->memory).are_keys_equals(curr_item, &key) &&
         hash < (env->memory).capacity) {
    hash++;
    curr_item = &items[hash];
  }

  if (curr_item->is_occupied &&
      !((env->memory).are_keys_equals(curr_item, &key))) {
    printf("OUT OF MEMORY IN DONKEYLANG\n");
    return;
  }

  // if i dont wanna support redeclaration with lets
  // just putting the assigmnet here would do the
  // job
  if (!curr_item->is_occupied) {
    env->memory.len++;
  }

  curr_item->key = key;
  curr_item->is_occupied = true;
  curr_item->value = value;
}

Object env_get_object(Arena *arena, Enviroment *env, String key) {
  HashTable memory_env = env->memory;
  if (!memory_env.items) {
    Object error_object = new_error(arena, "identifier not found: %S", key);
    return error_object;
  }

  U64 hash = get_hash(key) % (memory_env).capacity;
  KeyValueMemory *items = (memory_env).items;
  KeyValueMemory *curr_item = &items[hash];

  while (curr_item->is_occupied &&
         !(memory_env).are_keys_equals(curr_item, &key) &&
         hash < (memory_env).capacity) {
    hash++;
    curr_item = &items[hash];
  }

  bool are_keys_equals = (memory_env).are_keys_equals(curr_item, &key);

  if (!are_keys_equals) {
    Object error_object = new_error(arena, "identifier not found: %S", key);
    return error_object;
  }

  return curr_item->value;
}

void env_clone(Arena *arena, Enviroment *inner, Enviroment *outer) {
  /*memory_copy((byte *)inner, (byte *)&outer, sizeof(HashTable));*/
  HashTable outer_ht = outer->memory;
  hash_table_alloc(arena, &inner->memory, KeyValueMemory,
                   outer_ht.are_keys_equals);
  memory_copy((byte *)inner->memory.items, (byte *)outer_ht.items,
              outer_ht.item_size * outer_ht.capacity);
}
