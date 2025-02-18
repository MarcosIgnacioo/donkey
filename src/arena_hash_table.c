#ifndef _ARENA_HASH_TABLE_H
#define _ARENA_HASH_TABLE_H
#include "arena_strings.c"
#include <stdbool.h>
#include <time.h>

typedef unsigned long U64;
typedef long long I64;
typedef char byte;

typedef struct {
  const char *key;
  int value;
  bool is_occupied;
} KeyValue;

#define kv(KV_T, K, V)                                                         \
  (KV_T) { .key = K, .value = V, .is_occupied = true }

typedef bool (*predicator_fn)(void *, void *);

typedef struct {
  U64 len;
  U64 capacity;
  U64 item_size;
  void *items;
  predicator_fn are_keys_equals;
  predicator_fn are_values_equals;
} HashTable;

#define hash_table_alloc(_arena, _hash_table, _cap)                            \
  do {                                                                         \
    (_hash_table)->capacity = _cap;                                            \
    (_hash_table)->len = 0;                                                    \
    (_hash_table)->items =                                                     \
        arena_alloc(_arena, sizeof(*(_hash_table)->items) * _cap);             \
    memory_set((byte *)(_hash_table)->items,                                   \
               sizeof(*(_hash_table)->items) * _cap, 0);                       \
  } while (false)

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
      printf("table overflow\n");                                              \
    }                                                                          \
    (TABLE).items[hash].key = KEY;                                             \
    (TABLE).items[hash].value = VALUE;                                         \
    (TABLE).items[hash].is_occupied = true;                                    \
  } while (false)

void *hash_table_find_item(HashTable table, void *key) {
  byte *items = table.items;
  U64 i = 0;
  KeyValue *item_kv_cast = (KeyValue *)(items + i * table.item_size);
  for (; i < table.capacity; i++) {
    void *item_kv = items + i * table.item_size;
    item_kv_cast = (KeyValue *)(items + i * table.item_size);
    (void)item_kv_cast;
    if (table.are_keys_equals(item_kv, key)) {
      return item_kv;
    }
  }
  return NULL;
}

void *hash_table_find_key(HashTable table, void *value) {
  byte *items = table.items;
  for (U64 i = 0; i < table.capacity; i++) {
    if (table.are_values_equals(items + i * table.item_size, value)) {
      return items + i * table.item_size;
    }
  }
  return NULL;
}

U64 hash(String string);

U64 hash(String string) {
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
