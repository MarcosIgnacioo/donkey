#include "./repl.c"
#include <stdio.h>

typedef struct Foo Foo;



int main() {
  Arena arena = {0};
  donkey_repl(&arena);
}

