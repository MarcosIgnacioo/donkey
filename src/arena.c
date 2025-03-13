#ifndef _ARENA_H
#define _ARENA_H
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#define null NULL
#define nil NULL

#define REGION_DEFAULT_CAPACITY (8 * 1024)
#define ARENA_INITIAL_CAP 256
#define REGION_CAPACITY 128
#define ARENA_ASSERT assert
#define DOESNT_FIT_IN_LAST_REGION(ARENA, SIZE)                                 \
  ARENA->end->count + SIZE > ARENA->end->capacity
#define MEMORY_ADDRESS_ISNT_IN_THIS_REGION(MEMORY, REGION)                     \
  REGION->next &&                                                              \
      ((MEMORY) > REGION->data || (MEMORY) < REGION->data + REGION->capacity)
typedef unsigned long U64;
typedef unsigned long int U64_PTR;
typedef char byte;

typedef struct Region {
  struct Region *next;
  U64 count;
  U64 capacity;
  U64_PTR data[];
} Region;

typedef struct Arena {
  Region *begin;
  Region *end;
} Arena;

typedef struct Arena_Mark {
  Region *region;
  U64 count;
} Arena_Mark;

// this arena implementation doenst pop anything ajdsfkdsf, i mean i can make a
// pop version, but when doing reallocations the old_arrays are not 'freed',
// they are still being memory
Region *new_region(U64 capacity);
void free_region(Region *region);

void arena_pop_region(Arena *arena, void *);
void *arena_alloc(Arena *arena, U64 size_in_bytes);
void *arena_alloc_n_items(Arena *arena, U64 size);
void *arena_realloc(Arena *arena, void *old_arena_ptr, U64 old_size,
                    U64 new_size);
void *arena_string_dup(Arena *arena, const char *source);
void *arena_memory_dup(Arena *arena, void *data, U64 amount);
char *arena_sprintf(Arena *arena, const char *format, ...);

Arena_Mark arena_snapshot(Arena *arena);
Region *arena_get_region_from(Arena *arena, void *memory_address);
void arena_reset(Arena *arena);
void arena_rewind(Arena *arena, Arena_Mark m);
void arena_free(Arena *arena);
void arena_trim(Arena *arena);

void memory_copy(byte *this, const byte *that, int bytes);
void memory_set(byte *memory, U64 bytes, byte value);
U64 str_len(const char *str);
// strings
// this is not quite good btw but i like making the functions yay!
void memory_copy(byte *this, const byte *that, int bytes) {
  for (U64 i = 0; i < bytes; i++) {
    this[i] = that[i];
  }
};

void memory_set(byte *memory, U64 bytes, byte value) {
  for (U64 i = 0; i < bytes; i++) {
    memory[i] = value;
  }
}

Region *new_region(U64 capacity) {
  U64 total_size = sizeof(Region) + (sizeof(U64_PTR) * capacity);
  Region *region = malloc(total_size);
  ARENA_ASSERT(region);
  region->next = NULL;
  region->count = 0;
  region->capacity = capacity;
  return region;
}

Region *new_region_zero(U64 capacity) {
  U64 data_bytes = sizeof(U64_PTR) * capacity;
  U64 total_size = sizeof(Region) + data_bytes;
  Region *region = malloc(total_size);
  ARENA_ASSERT(region);
  region->next = NULL;
  region->count = 0;
  region->capacity = capacity;
  memory_set((byte *)region->data, data_bytes, 0);
  return region;
}

void free_region(Region *region) { free(region); }

// arena well arena
//
// size_in_bytes amount of bytes we want to allocate
void *arena_alloc(Arena *arena, U64 size_in_bytes) {
  U64 size = (size_in_bytes + (sizeof(U64_PTR) - 1)) / sizeof(U64_PTR);

  if (!arena->end) {
    ARENA_ASSERT(arena->begin == NULL);
    U64 capacity = REGION_DEFAULT_CAPACITY;
    if (capacity < size)
      capacity = size;
    arena->end = new_region(capacity);

    arena->begin = arena->end;
  }

  while (DOESNT_FIT_IN_LAST_REGION(arena, size) //
         && arena->end->next != NULL) {
    arena->end = arena->end->next;
  }

  if (DOESNT_FIT_IN_LAST_REGION(arena, size)) {
    ARENA_ASSERT(arena->end->next == NULL);
    U64 capacity = REGION_DEFAULT_CAPACITY;
    if (capacity < size) {
      capacity = size;
    }
    arena->end->next = new_region(capacity);
    arena->end = arena->end->next;
  }

  void *memory = &arena->end->data[arena->end->count];
  memory_set((byte *)memory, size, 0);
  arena->end->count += size;
  return memory;
}

void *arena_alloc_n_items(Arena *arena, U64 size) {
  if (!arena->end) {
    ARENA_ASSERT(arena->begin == NULL);
    U64 capacity = REGION_DEFAULT_CAPACITY;
    if (capacity < size)
      capacity = size;
    arena->end = new_region(capacity);
    arena->begin = arena->end;
  }

  while (DOESNT_FIT_IN_LAST_REGION(arena, size) //
         && arena->end->next != NULL) {
    arena->end = arena->end->next;
  }

  if (DOESNT_FIT_IN_LAST_REGION(arena, size)) {
    ARENA_ASSERT(arena->end->next == NULL);
    U64 capacity = REGION_DEFAULT_CAPACITY;
    if (capacity < size) {
      capacity = size;
    }
    arena->end->next = new_region(capacity);
    arena->end = arena->end->next;
  }

  void *memory = &arena->end->data[arena->end->count];
  arena->end->count += size;
  return memory;
}

void *arena_alloc_zero(Arena *arena, U64 size_in_bytes) {
  U64 size = (size_in_bytes + (sizeof(U64_PTR) - 1)) / sizeof(U64_PTR);

  if (!arena->end) {
    ARENA_ASSERT(arena->begin == NULL);
    U64 capacity = REGION_DEFAULT_CAPACITY;
    if (capacity < size)
      capacity = size;
    arena->end = new_region_zero(capacity);

    arena->begin = arena->end;
  }

  while (DOESNT_FIT_IN_LAST_REGION(arena, size) //
         && arena->end->next != NULL) {
    arena->end = arena->end->next;
  }

  if (DOESNT_FIT_IN_LAST_REGION(arena, size)) {
    ARENA_ASSERT(arena->end->next == NULL);
    U64 capacity = REGION_DEFAULT_CAPACITY;
    if (capacity < size) {
      capacity = size;
    }
    arena->end->next = new_region_zero(capacity);
    arena->end = arena->end->next;
  }

  void *memory = &arena->end->data[arena->end->count];
  memory_set((byte *)memory, size, 0);
  arena->end->count += size;
  return memory;
}

void *arena_realloc(Arena *arena, void *old_arena_ptr, U64 old_size,
                    U64 new_size) {

  if (old_size >= new_size) {
    return old_arena_ptr;
  }

  byte *old_arena_byte_ptr = (byte *)old_arena_ptr;
  byte *new_arena_byte_ptr = (byte *)arena_alloc(arena, new_size);

  for (U64 i = 0; i < old_size; i++) {
    new_arena_byte_ptr[i] = old_arena_byte_ptr[i];
  }

  /*Region *forked_region = arena_get_region_from(arena, old_arena_ptr);*/
  /*forked_region->count;*/
  return new_arena_byte_ptr;
}

void *arena_string_dup(Arena *arena, const char *source) {
  U64 source_len = str_len(source);
  byte *str_dupped = arena_alloc(arena, source_len + 1);
  memory_copy(str_dupped, source, source_len);
  str_dupped[source_len] = '\0';
  return str_dupped;
}

void *arena_memory_dup(Arena *arena, void *source, U64 amount) {
  byte *mem_dupped = arena_alloc(arena, amount);
  memory_copy(mem_dupped, source, amount);
  return mem_dupped;
  /*return memory_copy(arena_alloc(arena, amount), source, amount);*/
}

char *arena_sprintf(Arena *arena, const char *format, ...) {
  va_list args;
  va_start(args, format);
  int n = vsnprintf(NULL, 0, format, args);
  va_end(args);

  ARENA_ASSERT(n >= 0);
  char *result = (char *)arena_alloc(arena, n + 1);
  va_start(args, format);
  vsnprintf(result, n + 1, format, args);
  va_end(args);

  return result;
}

void arena_free(Arena *arena) {
  Region *curr = arena->begin;
  while (curr) {
    Region *freeing_region = curr;
    curr = curr->next;
    free_region(freeing_region);
  }
  arena->begin = NULL;
  arena->end = NULL;
}

Region *arena_get_region_from(Arena *arena, void *memory_address) {
  Region *region;
  for (region = arena->begin;
       MEMORY_ADDRESS_ISNT_IN_THIS_REGION((U64_PTR *)memory_address, region);
       region = region->next)
    ;

  if (!region) {
    ARENA_ASSERT(arena->begin == NULL);
  }

  return region;
}

Arena_Mark arena_snapshot(Arena *arena) {
  Arena_Mark mark = (Arena_Mark){0};
  if (arena->end) {
    mark = (Arena_Mark){.region = arena->end, .count = arena->end->count};
  } else {
    // i think we assert here becasue the end can never not be null unless the
    // arena is unitiliazed so we do assertiion here in the begin
    ARENA_ASSERT(arena->begin ==
                 NULL); // if the end arena is null we might check the beginigng
                        // one, so if, there is one
  }
  return mark;
}

// fix this cause it keeps the count from some regions
void arena_reset(Arena *arena) {
  Region *curr = arena->begin;

  while (curr) {
    curr->count = 0;
    curr = curr->next;
  }

  arena->end = arena->begin;
}

void arena_rewind(Arena *arena, Arena_Mark m) {
  if (m.region == NULL) {
    arena_reset(arena);
    return;
  }

  m.region->count = m.count;

  for (Region *curr = arena->end; curr; curr = curr->next) {
    curr->count = 0;
  }

  arena->end = m.region;
}

void arena_trim(Arena *arena) {
  Region *region = arena->end->next;
  while (region) {
    Region *freeing_region = region;
    region = region->next;
    free(freeing_region);
  }
  arena->end->next = NULL;
};

#endif /* ifndef _ARENA_H */
