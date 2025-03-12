#include "./arena_hash_table.c"
#include <stdbool.h>
#include <stdio.h>

typedef struct {
  String key;
  bool is_occupied;
  union {
    int int_value;
    String string_value;
  };
} KeyValueDonkey;

bool key_value_donkey_fn(void *a, void *b) {
  String a_string = (*(KeyValueDonkey *)a).key;
  String b_string = *(String *)b;
  return string_equals(a_string, b_string);
}

HashIndexResult get_KVD(void *void_items, U64 hash) {
  KeyValueDonkey *items = (KeyValueDonkey *)void_items;
  String key = items[hash].key;
  bool is_occupied = items[hash].is_occupied;

  HashResult result = (HashIndexResult){
      .item_ptr = items + hash,
      .is_occupied = is_occupied,
  };

  return result;
}

void set_KVD(void *void_items, U64 hash, void *key, )

    int main() {
  Arena arena = {0};
  HashTable ht = {0};
  /*void *PROGRAM_MEMORY = arena_alloc(&arena, 1024 * 8);*/
  printf("%p\n", ht.items);
  hash_table_alloc(&arena, &ht, KeyValueDonkey, &key_value_donkey_fn);
  String KEY = string("lazy o lazy im losign it lately");
  int VALUE = 123;
  do {
    U64 hash = (ht).get_hash(KEY) % cap((ht).items);
    byte *items = (ht).items;
    byte *cursor = items;
    bool is_occupied = *(bool *)(cursor + sizeof(String));
    while (is_occupied && !(ht).are_keys_equals(cursor, &KEY) &&
           hash < cap((ht).items)) {
      hash++;
      cursor += (ht).item_size * hash;
      is_occupied = *(bool *)cursor + sizeof(String);
    }

    if (is_occupied && !((ht).are_keys_equals(cursor, &KEY))) {
      printf("OUT OF MEMORY IN DONKEYLANG\n");
    }

    cursor.int_value = VALUE;
    cursor.key = KEY;
    cursor.is_occupied = true;
  } while (false);
  return 0;
}
