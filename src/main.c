#include "arena_strings.c"
#include <stdio.h>

typedef enum { ONE, TWO, THREE } foo;

#define h_m(K_T, V_T, NAME)                                                    \
  struct {                                                                     \
    K_T key;                                                                   \
    V_T value;                                                                 \
  } *name = NULL

#define stbds_shput(t, k, v)                                                   \
  ((t) = stbds_hmput_key_wrapper((t), sizeof *(t), (void *)(k),                \
                                 sizeof(t)->key, STBDS_HM_STRING),             \
   (t)[stbds_temp((t) - 1)].value = (v))
#define shput(HM, KEY, VALUE)                                                  \
  do {                                                                         \
    append(HM->items, (KeyValue))                                                      \
  } while (false)

int main() {
  Arena arena = (Arena){.begin = NULL, .end = NULL};

  struct {
    char * key;
    char value;
  } *table = NULL;

  char name[4] = "jen";
  shput(hash, "bob", 'h');
  shput(hash, "sally", 'e');
  shput(hash, "fred", 'l');
  shput(hash, "jen", 'x');
  shput(hash, "doug", 'o');

  shput(hash, name, 'l');
  shfree(hash);
  /*String this = arena_new_string(&arena, "hola mundeee");*/
  /*printfln("%S", this);*/
  arena_free(&arena);
  return 0;
}
