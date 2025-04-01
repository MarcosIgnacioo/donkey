#include "./test.h"

typedef struct {
  Object key;
  I64 expected;
} TestHashMapuwu;

#include "../object/object_hashmap.c"

typedef struct {
  int *arr;
  int x;
} Sanity;

void test_hash_map() {
  Arena arena = (Arena){.begin = NULL, .end = NULL};
  char *input = "let two = \" two \";"
                "{"
                "\" one \": 10 - 9,"
                "two: 1 + 1,"
                "\" thr \" + \" ee \": 6 / 2,"
                "4: 4,"
                "true: 5,"
                "false: 6"
                "}";
  char *inputs[] = {input};
  TestHashMapuwu test_keys[] = {
      {.key =
           {
               .type = STRING_OBJECT,
               .string.value = string(" one "),
           },
       .expected = 1},
      {.key =
           {
               .type = STRING_OBJECT,
               .string.value = string(" two "),
           },
       .expected = 2},
      {.key =
           {
               .type = STRING_OBJECT,
               .string.value = string(" thr  ee "),
           },
       .expected = 3},
      {.key =
           {
               .type = INTEGER_OBJECT,
               .integer.value = 4,
           },
       .expected = 4},
      {.key =
           {
               .type = BOOLEAN_OBJECT,
               .boolean.value = true,
           },
       .expected = 5},
      {.key =
           {
               .type = BOOLEAN_OBJECT,
               .boolean.value = false,
           },
       .expected = 6},
  };
  Enviroment env = {0};
  bool pass = true;
  for (size_t i = 0; i < array_len(inputs); i += 1) {
    String input = string(inputs[i]);
    Lexer lexer = lexer_new_lexer(input);
    Parser parser = ast_new_parser(&arena, &lexer);
    Program program = ast_parse_program(&arena, &parser);
    print_parser_errors(parser);
    Object testing = eval_evaluate_program(&arena, &env, program);
    if (testing.type != HASH_MAP_OBJECT) {
      printf("hahahahah is not a hashmapp[ppppp fj]");
    }
    HashTable *hm = testing.hash_map.value;
    for (size_t j = 0; j < array_len(test_keys); j++) {
      Object key = test_keys[j].key;
      I64 expected = test_keys[j].expected;
      Object got = object_hash_map_get_item(hm, key);
      if (!test_object_integer(got, expected)) {
        printfln("FAILED:%S", input);
        printf("expected:%lld\n", expected);
        printfln("got:%S\n", object_to_string(&arena, got));
        pass = false;
      }
      String obj_str = object_to_string(&arena, got);
      printfln("%S", obj_str);
    }
    printf("\n");
  }
  if (pass) {
    printfln(LOG_SUCCESS "success");
  }
}
