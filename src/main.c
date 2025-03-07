#include <stdbool.h>
#include <stdio.h>

void fuzz(unsigned long long x) {
  printf("%llu\n", x);
}

int main() {
  bool foo = false;
  short buzz = 123;
  (void) buzz;
  fuzz(foo);
  long long a1 = (long long)&foo;
  long long a2 = (long long)&buzz;
  printf("%p\n", &foo);
  printf("%p\n", &buzz);
  /*printf("%lld\n", a2 - a1);*/
  /*printf("%zu\n", sizeof(int));*/
  /*Arena arena = {0};*/
  /*donkey_repl(&arena);*/
}
