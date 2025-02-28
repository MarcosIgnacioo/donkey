#include <stdio.h>

int main() {
  for (int i = 0; i < 24; i++) {
    if (i > 0) {
      printf(",");
    }
    printf("%d", i);
  }
  printf("hello world\n");
  /*why it doesnt overflow to zero ?*/
  /*I64 res = string_to_integer_64(string("-2147483649"));*/
  /*printf("%d\n", res);*/
}
