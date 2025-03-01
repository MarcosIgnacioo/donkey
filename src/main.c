#include "./repl.c"
#include <stdio.h>

int main() {
  Arena arena = {0};
  donkey_repl(&arena);
}
