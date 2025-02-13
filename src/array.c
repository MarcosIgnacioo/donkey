// source
// https://bytesbeneath.com/articles/dynamic-arrays-in-c
#ifndef _ARRAY_H
#define _ARRAY_H
#include "arena.c"
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ARRAY_CAPACITY 512
#define arena_array(A, T) array_init(A, sizeof(T), ARRAY_CAPACITY)
#define arena_array_with_cap(A, T, C) array_init(A, sizeof(T), C)
#define head(array) (((Array_Header *)array) - 1)
#define len(array) ((head(array))->length)
#define array_len(array) (sizeof array / sizeof array[0])
#define cap(array) ((head(array))->capacity)

#define append(a, v)                                                           \
  ((a) = array_ensure_capacity(a, 1, sizeof(*a)), (a)[head(a)->length] = (v),  \
   &(a)[head(a)->length++])

#define remove_at(array, idx)                                                  \
  do {                                                                         \
    Array_Header *header = head(array);                                        \
    int last_idx = header->length - 1;                                         \
    if (last_idx == idx) {                                                     \
      header->length--;                                                        \
    } else if (header->length > 1) {                                           \
      void *free_space = &array[idx];                                          \
      void *last = &array[last_idx];                                           \
      memcpy(free_space, last, sizeof(*array));                                \
      header->length--;                                                        \
    }                                                                          \
  } while (0);

#define pop_back(array) ((array)[head(array)->length])
#define reset(array) (head(array)->length = 0)

typedef union {
  size_t padding;
  Arena *arena;
} Context;
typedef struct {
  size_t length;
  size_t capacity;
  Context context;
} Array_Header;

void *array_init(Arena *arena, size_t item_size, size_t capacity) {
  void *ptr = 0;
  size_t size = sizeof(Array_Header) + item_size * capacity;
  Array_Header *header = arena_alloc(arena, size);
  ARENA_ASSERT(header);
  header->capacity = capacity;
  header->length = 0;
  header->context.arena = arena;
  ptr = header + 1;
  return ptr;
}

void *array_ensure_capacity(void *array, size_t item_count, size_t item_size) {
  Array_Header *header = head(array);

  if (!header)
    return NULL;

  size_t desired_capacity = header->length + item_count;

  if (header->capacity >= desired_capacity) {
    return header + 1;
  }

  size_t new_capacity = header->capacity * 2;

  while (new_capacity < desired_capacity)
    new_capacity = header->capacity * 2;

  size_t old_size = sizeof(*header) + header->length * item_size;
  size_t new_size = old_size + new_capacity * item_size;

  Array_Header *reallocated_header =
      arena_alloc(header->context.arena, new_size);

  if (!reallocated_header)
    return NULL;

  memcpy(reallocated_header, header, old_size);
  reallocated_header->capacity = new_capacity;
  // the arena is the one doing the memory management now
  /*free(header);*/
  return reallocated_header + 1;
}
#endif /* ifndef _ARRAY_H */
