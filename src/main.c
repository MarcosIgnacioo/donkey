#include "arena_strings.c"
#include <stdio.h>

typedef enum { ONE, TWO, THREE } foo;

int main() {
  printf("%zu\n", sizeof(ONE));
  /*Arena arena = (Arena){.begin = NULL, .end = NULL};*/
  /*String this = arena_new_string(&arena, "hola mundeee");*/
  /*printfln("%S", this);*/
  /*arena_free(&arena);*/
  return 0;
}
