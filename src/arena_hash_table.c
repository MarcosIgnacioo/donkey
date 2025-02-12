#ifndef _ARENA_HASH_TABLE_H
#define _ARENA_HASH_TABLE_H
#include "arena_strings.c"
#include <stdbool.h>
#include <time.h>

typedef unsigned long U64;
typedef long long I64;
typedef char byte;

typedef struct {
  String key;
  int value;
  bool is_occupied;
} KeyValue;

typedef struct {
  U64 len;
  U64 capacity;
  KeyValue *items;
} HashTable;

#define hash_table_alloc(_arena, _hash_table, _cap)                            \
  do {                                                                         \
    (_hash_table)->capacity = _cap;                                              \
    (_hash_table)->len = 0;                                                      \
    (_hash_table)->items =                                                       \
        arena_alloc(_arena, sizeof(*(_hash_table)->items) * _cap);               \
    memory_set((byte *)(_hash_table)->items, sizeof(*(_hash_table)->items) * _cap, 0);      \
  } while (false)

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

int compare(const void *a, const void *b) {
  KeyValue a_kv = *(KeyValue *)a;
  KeyValue b_kv = *(KeyValue *)b;
  return (b_kv.value - a_kv.value);
}

void print_top_n(KeyValue *tokens_filtered, U64 N) {
  qsort(tokens_filtered, len(tokens_filtered), sizeof(*tokens_filtered),
        &compare);
  for (size_t i = 0; i < N; i++) {
    KeyValue token = tokens_filtered[i];
    printfln("%lu : %S => %lu", i, token.key, token.value);
  }
}

#endif /* ifndef _                                                             \
        */
