#ifndef _OBJECT_HASH_MAP_H
#define _OBJECT_HASH_MAP_H
#include "../arena_hash_table.c"
#include "../parser/ast.c"
#include <stdbool.h>

// one nut idea would be to stringify all the expressions and just compare the
// strings asdkfasdf which is a 'good' idea if i just wanna end with this but i
// spent so much time writing the parser so lets feel it in our hands if u could
// see i am the one who understands youuu uuu belong with meeeeee eyou belong
// with me eeeeeeeeee walk in the streeets with you and your worn out jeans
// laguing in a park bench hey isnt this easy light up this whole town in a
// while since she boroguh u donwwwwwwwww girtl like that i wear cheer captain
// dreaming about the day olloo kin is askdjfkasdjfsd
typedef struct {
  Object key;
  bool is_occupied;
  Object value;
} KeyValueObject;
bool are_objects_equals(void *a_ptr, void *b_ptr);

#include "./object.h"
// im beggin u please make an arena tmp implementation where
// when it hits the limit it just resets itself
// PLEASE
// ok
U64 object_get_hash(void *key_ptr) {
  Object expression = *(Object *)key_ptr;
  Arena tmp_arena = {0};
  String key = object_to_string(&tmp_arena, expression);
  U64 d = 0x01000193;
  for (U64 i = 0; i < key.len; i++) {
    d = ((d * 0x01000193) ^ key.str[i]) & 0xffffffff;
  }
  arena_free(&tmp_arena);
  return d;
}

Object object_hash_map_get_item(HashTable *table, Object key) {
  if (!table || !(table)->items) {
    return (Object){0};
  }

  U64 hash = (table)->get_hash(&key) % (table)->capacity;
  KeyValueObject *items = (table)->items;
  KeyValueObject *curr_item = &items[hash];

  while (curr_item->is_occupied &&
         !(table)->are_keys_equals(&curr_item->key, &key) &&
         hash < (table)->capacity) {
    hash++;
    curr_item = &items[hash];
  }

  bool are_keys_equals = (table)->are_keys_equals(&curr_item->key, &key);

  if (!are_keys_equals) {
    return (Object){0};
  }

  return curr_item->value;
}

void object_hash_map_insert(Arena *arena, HashTable *table, Object key,
                            Object value) {

  if (!table->items) {
    hash_table_alloc_custom(
      arena,
      table,
      KeyValueObject,
      are_objects_equals,
      object_get_hash
    );
  }

  if (table->len >= table->capacity) {
    table->items =
        realloc(table->items, table->capacity * 2 * table->item_size);
    table->capacity = table->capacity * 2 * table->item_size;
  }

  // xd
  U64 hash = table->get_hash(&key) % (table->capacity);
  KeyValueObject *items = (table)->items;
  KeyValueObject *curr_item = &items[hash];

  while (curr_item->is_occupied &&
         !(table)->are_keys_equals(&curr_item->key, &key) &&
         hash < (table)->capacity) {
    hash++;
    curr_item = &items[hash];
  }

  if (curr_item->is_occupied &&
      !((table)->are_keys_equals(&curr_item->key, &key))) {
    printf("OUT OF MEMORY IN DONKEYLANG\n");
    return;
  }

  // if i dont wanna support redeclaration with lets
  // just putting the assigmnet here would do the
  // job
  if (!curr_item->is_occupied) {
    table->len++;
  }

  curr_item->key = key;
  curr_item->is_occupied = true;
  curr_item->value = value;
}

bool are_object_hash_maps_equals(ObjectHashMap a, ObjectHashMap b) {
  if (!a.value || !b.value) {
    return false;
  }

  bool are_key_values_equals = false;
  KeyValueObject *items_a = (KeyValueObject *)a.value->items;

  for (size_t i = 0; i < a.value->capacity; i++) {
    KeyValueObject item_a = items_a[i];
    if (!item_a.is_occupied) {
      continue;
    }
    Object value_in_b = object_hash_map_get_item(b.value, item_a.key);
    bool are_values_equals = are_objects_equals(&item_a.value, &value_in_b);
    if (are_values_equals) {
      are_key_values_equals = true;
      break;
    }
  }

  return are_key_values_equals;
}

bool are_object_integers_equals(ObjectInteger integer_a,
                                ObjectInteger integer_b) {
  return integer_a.value == integer_b.value;
}
bool are_object_booleans_equals(ObjectBoolean boolean_a,
                                ObjectBoolean boolean_b) {
  return boolean_a.value == boolean_b.value;
}
bool are_object_strings_equals(ObjectString string_a, ObjectString string_b) {
  return string_equals(string_a.value, string_b.value);
}

bool are_object_arrays_equals(ObjectArray array_a, ObjectArray array_b) {
  bool are_equals = true;
  if (len(array_a.value) != len(array_b.value)) {
    return false;
  }

  for (size_t i = 0; i < len(array_a.value); i++) {
    Object a = array_a.value[i];
    Object b = array_b.value[i];
    if (!are_objects_equals(&a, &b)) {
      are_equals = false;
      break;
    }
  }

  return are_equals;
}
#include "../parser/donkey_hashmap.c"

bool are_object_functions_equals(ObjectFunction function_a,
                                 ObjectFunction function_b) {
  if (len(function_a.parameters) != len(function_b.parameters)) {
    return false;
  }
  bool are_names_equals = string_equals(function_a.name, function_b.name);
  bool are_parameters_equals = true;
  for (size_t i = 0; i < len(function_a.parameters); i++) {
    Identifier identifier_a = function_a.parameters[i];
    Identifier identifier_b = function_b.parameters[i];
    if (!are_identifier_equals(identifier_a, identifier_b)) {
      are_parameters_equals = false;
      break;
    }
  }

  bool are_bodies_equals =
      are_block_statements_equals(function_a.body, function_b.body);

  return are_names_equals && are_parameters_equals && are_bodies_equals;
}

bool are_objects_equals(void *a_ptr, void *b_ptr) {
  Object a = *(Object *)a_ptr;
  Object b = *(Object *)b_ptr;
  if (a.type != b.type) {
    return false;
  }
  switch (a.type) {
  case INTEGER_OBJECT:
    //
    {
      return are_object_integers_equals(a.integer, b.integer);
      break;
    }
  case BOOLEAN_OBJECT:
    //
    {
      return are_object_booleans_equals(a.boolean, b.boolean);
      break;
    }
  case STRING_OBJECT:
    //
    {
      return are_object_strings_equals(a.string, b.string);
      break;
    }
  case ARRAY_OBJECT:
    //
    {
      return are_object_arrays_equals(a.array, b.array);
      break;
    }
  case HASH_MAP_OBJECT:
    //
    {
      return are_object_hash_maps_equals(a.hash_map, b.hash_map);
      break;
    }
  case FUNCTION_OBJECT:
    //
    {
      return are_object_functions_equals(a.function, b.function);
      break;
    }
  default:
    //
    {
      return false;
    }
  }
}

// TODO: hashmap cool implementation use void pointers and because we have a
// equals function we dont actually need them to be a fixed type

// hashtable for object implementation

#include "./object.h"

void foo() { printf("sadf"); }

#endif /* ifndef _OBJECT_HASH_MAP_H */
