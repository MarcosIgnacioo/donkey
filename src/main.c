#include "arena_strings.c"
#include <stdio.h>
typedef enum {
  FOO,
  BELONG,
  WITH,
  ME,
  BAR,
} Easy;

typedef struct {
  Easy field;
} Fuzz;

#define stringify_enum(STR, ENUM) (STR, #ENUM)

int main() {
  /*why it doesnt overflow to zero ?*/
  /*I64 res = string_to_integer_64(string("-2147483649"));*/
  /*printf("%d\n", res);*/
}
