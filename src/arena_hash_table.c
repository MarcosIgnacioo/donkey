#ifndef _ARENA_HASH_TABLE_H
#define _ARENA_HASH_TABLE_H
#include "arena_strings.c"
#include <stdbool.h>
#include <time.h>

typedef struct {
  const char *key;
  int value;
  bool is_occupied;
} KeyValue;
#define kv(KV_T, K, V)                                                         \
  (KV_T) { .key = K, .value = V, .is_occupied = true }

typedef unsigned long U64;
typedef long long I64;
typedef char byte;

typedef bool (*predicator_fn)(void *, void *);
typedef U64 (*hash_fn)(String);
typedef struct {
  void *item_ptr;
  void *key;
  bool is_occupied;
} HashResult;
typedef HashResult (*get_item_in_hash_index)(void *, U64);

typedef struct {
  U64 len;
  U64 capacity;
  U64 item_size;
  void *items;
  predicator_fn are_keys_equals;
  predicator_fn are_values_equals;
  hash_fn get_hash;
} HashTable;

// stolen from https://stevehanov.ca/blog/?id=119 looks liek a good source
// Use the FNV algorithm from http://isthe.com/chongo/tech/comp/fnv/
U64 get_hash(String key) {
  U64 d = 0x01000193;
  for (U64 i = 0; i < key.len; i++) {
    d = ((d * 0x01000193) ^ key.str[i]) & 0xffffffff;
  }
  return d;
}

U64 cstr_get_hash(char *key) {
  U64 d = 0x01000193;
  for (U64 i = 0; key[i]; i++) {
    d = ((d * 0x01000193) ^ key[i]) & 0xffffffff;
  }
  return d;
}

#define DEFAULT_CAPACITY 1024 * 8

#define hash_table_alloc(_arena, _hash_table, _key_value_t, _are_keys_equals)  \
  do {                                                                         \
    (_hash_table)->capacity = DEFAULT_CAPACITY;                                \
    (_hash_table)->item_size = sizeof(_key_value_t);                           \
    (_hash_table)->are_keys_equals = _are_keys_equals;                         \
    (_hash_table)->get_hash = &get_hash;                                       \
    (_hash_table)->len = 0;                                                    \
    (_hash_table)->items =                                                     \
        arena_alloc(_arena, sizeof(_key_value_t) * DEFAULT_CAPACITY);          \
    memory_set((byte *)(_hash_table)->items,                                   \
               sizeof(_key_value_t) * DEFAULT_CAPACITY, 0);                    \
  } while (false) // 54: this sizeof(*(_hash_table)->items) should segfault use
                  // sizeof(_key_value_t) instead

#define ht_insert(TABLE, KEY, VALUE)                                           \
  do {                                                                         \
    U64 hash = (TABLE).get_hash(KEY) % cap((TABLE).items);                     \
    while ((TABLE).items[hash].is_occupied &&                                  \
           !(TABLE).are_keys_equals((TABLE).items[hash].key, KEY) &&           \
           hash < cap((TABLE).items)) {                                        \
      hash++;                                                                  \
    }                                                                          \
    if ((TABLE).items[hash].is_occupied &&                                     \
        !((TABLE).are_keys_equals((TABLE).items[hash].key, KEY))) {            \
      printf("OUT OF MEMORY IN DONKEYLANG\n");                                 \
    }                                                                          \
    (TABLE).items[hash].key = KEY;                                             \
    (TABLE).items[hash].value = VALUE;                                         \
    (TABLE).items[hash].is_occupied = true;                                    \
  } while (false)

#define ht_insert_lazy(TABLE, KEY, VALUE, VALUE_FIELD)                         \
  do {                                                                         \
    U64 hash = (TABLE).get_hash(KEY) % cap((TABLE).items);                     \
    while ((TABLE).items[hash].is_occupied &&                                  \
           !(TABLE).are_keys_equals((TABLE).items[hash].key, KEY) &&           \
           hash < cap((TABLE).items)) {                                        \
      hash++;                                                                  \
    }                                                                          \
    if ((TABLE).items[hash].is_occupied &&                                     \
        !((TABLE).are_keys_equals((TABLE).items[hash].key, KEY))) {            \
      printf("OUT OF MEMORY IN DONKEYLANG\n");                                 \
    }                                                                          \
    (TABLE).items[hash].key = KEY;                                             \
    (TABLE).items[hash].int_value = VALUE;                                     \
    (TABLE).items[hash].is_occupied = true;                                    \
  } while (false)

// TODO: make this work with the hash function
void *hash_table_find_item(HashTable table, void *key) {
  byte *items = table.items;
  void *item_kv;
  U64 i;
  for (i = 0; i < table.capacity; i++) {
    item_kv = items + i * table.item_size;
    if (table.are_keys_equals(item_kv, key)) {
      return item_kv;
    }
  }
  return NULL;
}

void *hash_table_find_item_key_string(HashTable table, String key) {
  byte *items = table.items;
  void *item_kv;
  U64 i;
  for (i = 0; i < table.capacity; i++) {
    item_kv = items + i * table.item_size;
    if (table.are_keys_equals(item_kv, &key)) {
      return item_kv;
    }
  }
  return NULL;
}

void *hash_table_find_key(HashTable table, void *value) {
  byte *items = table.items;
  void *item_kv;
  U64 i;
  for (i = 0; i < table.capacity; i++) {
    item_kv = items + i * table.item_size;
    if (table.are_values_equals(item_kv, value)) {
      return item_kv;
    }
  }
  return NULL;
}

U64 hash(String string);

size_t hash(String string) {
  U64 index = 0;
  for (U64 i = 0; i < string.len; i++) {
    index = index * 7 + string.str[i];
  }
  return index;
}

double delta_secs(struct timespec begin, struct timespec end) {
  double a = (double)begin.tv_sec + (double)begin.tv_nsec * 1e-9;
  double b = (double)end.tv_sec + (double)end.tv_nsec * 1e-9;
  return b - a;
}

void start_profiling(struct timespec *begin) {
  int time_error = clock_gettime(CLOCK_MONOTONIC, begin);
  assert(time_error == 0);
}

double finish_profiling(struct timespec begin) {
  struct timespec end = {0};
  int time_error = clock_gettime(CLOCK_MONOTONIC, &end);
  assert(time_error == 0);
  return delta_secs(begin, end);
}
#endif /* ifndef _                                                             \
        */
