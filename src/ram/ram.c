#include "./ram.h"
void bind_string_value_to_variable_in_ram(byte *item, int hash, String value) {
  KeyValueStrings key_value = *(KeyValueStrings *)item;
  key_value.value = value;
  key_value.is_occupied = true;
}

void bind_integer_value_to_variable_in_ram(byte *item, int hash, int value) {
  KeyValueIntegers key_value = *(KeyValueIntegers *)item;
  key_value.value = value;
  key_value.is_occupied = true;
}

bool RAM_key_value_equals(void *this, void *that) {
  /*
    `this` is actually the keyvalue,
    but because i wanna use this
    function with all the keyvalues
    of the hashmap i do magic casting
    the `this` pointer to a String * and
    then dereference it because it is always
    the first element in the struct
  */
  String this_str = *(
      String *)this; // rust devs must be wilding rn im sorry but not that much
  String that_str = *(String *)that;
  return string_equals(this_str, that_str);
}

// one hashmap for strings
// one hashmap for integers

byte *get_item_in_hash_table(HashTable ht, String key) {
  U64 hash = get_hash(key) % (ht).capacity;
  byte *items = (ht).items;
  byte *cursor = items + ((ht).item_size * hash);
  bool is_occupied = *(bool *)(cursor + sizeof(String));
  while (is_occupied && !(ht).are_keys_equals(cursor, &key) &&
         hash < (ht).capacity) {
    hash++;
    cursor += (ht).item_size * hash;
    is_occupied = *(bool *)cursor + sizeof(String);
  }

  if (is_occupied && !((ht).are_keys_equals(cursor, &key))) {
    printf("OUT OF MEMORY IN DONKEYLANG\n");
    return NULL;
  }
  return cursor;
}

KeyValueStrings *insert_string_to_memory(HashTable ht, String key,
                                         String value) {
  void *item_ptr = get_item_in_hash_table((ht), key);
  if (!item_ptr) {
    return NULL;
  }
  KeyValueStrings *item = (KeyValueStrings *)item_ptr;
  item->key = key;
  item->value = value;
  return item;
}

KeyValueIntegers *insert_integers_to_memory(HashTable ht, String key,
                                            int value) {
  void *item_ptr = hash_table_find_item((ht), &key);
  if (!item_ptr) {
    return NULL;
  }
  KeyValueIntegers *item = (KeyValueIntegers *)item_ptr;
  item->key = key;
  item->value = value;
  return item;
}

String get_string_from_memory(HashTable ht, String key) {
  void *item_ptr = hash_table_find_item((ht), &key);
  if (!item_ptr) {
    return string("(donk)");
  }
  KeyValueStrings *item = (KeyValueStrings *)item_ptr;
  return item->value;
}

int get_integer_from_memory(HashTable ht, String key) {
  void *item_ptr = hash_table_find_item((ht), &key);
  if (!item_ptr) {
    return 0;
  }
  KeyValueIntegers *item = (KeyValueIntegers *)item_ptr;
  return item->value;
}
