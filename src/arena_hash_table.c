#ifndef _ARENA_HASH_TABLE_H
#define _ARENA_HASH_TABLE_H
#include "arena_strings.c"
#include <stdbool.h>
#include <time.h>

typedef unsigned long U64;
typedef long long I64;
typedef char byte;

typedef struct {
  const char* key;
  U64 value;
  bool is_occupied;
} KeyValue;

#define kv(K, V)                                                               \
  (KeyValue) { .key = K, .value = V, .is_occupied = true }

typedef struct {
  U64 len;
  U64 capacity;
  KeyValue *items;
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

int hash_table_find_item(HashTable table, String key) {
  for (U64 i = 0; i < table.capacity; i++) {
    if (c_string_equals(table.items[i].key, key.str)) {
      return table.items[i].value;
    }
  }
  return 0;
}

const char * hash_table_find_key(HashTable table, int value) {
  for (U64 i = 0; i < table.capacity; i++) {
    if (table.items[i].value == value) {
      return table.items[i].key;
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
