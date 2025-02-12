#include "arena_strings.c"
#include <stdio.h>

int main() {
  Arena arena = (Arena) {.begin = NULL, .end = NULL};
  String this = arena_new_string(&arena, "hola mundeee");
  printfln("%S",this);
  arena_free(&arena);
  return 0;
}
