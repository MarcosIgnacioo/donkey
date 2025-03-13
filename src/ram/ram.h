#ifndef _ENV_H
#define _ENV_H
#include "../arena_hash_table.c"
#include "../object/object.h"

typedef struct {
  HashTable memory;
} Enviroment;

typedef struct {
  String key;
  bool is_occupied;
  Object value;
} KeyValueMemory;


/*HashTable MEMORY = {0};*/

void env_init(Arena *, Enviroment *);
bool env_key_value_equals(void *, void *);
void env_insert_object(Arena *, Enviroment, String, Object);
Object env_get_object(Arena *, Enviroment, String);
#endif // !_ENV_H
