#ifndef _ENV_H
#define _ENV_H
#include "../arena_hash_table.c"
#include "../object/object.h"

struct Enviroment {
  HashTable memory;
  Enviroment *outer_memory;
};

typedef struct {
  String key;
  bool is_occupied;
  Object value;
} KeyValueMemory;

/*HashTable MEMORY = {0};*/

void env_init(Arena *, Enviroment *);
bool env_key_value_equals(void *, void *);
void env_insert_object(Arena *, Enviroment *, String, Object);
void env_clone(Arena *, Enviroment *, Enviroment *);
void env_reset_local(Enviroment *env);
Object env_get_object(Arena *, Enviroment *, String);
Object _env_get_object(Arena *, Enviroment *, String);
#endif // !_ENV_H
//
