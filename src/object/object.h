#ifndef _OBJECT_H
#define _OBJECT_H
#include "../arena_strings.c"
#include "../parser/ast.c"
#include <stdbool.h>

#define OBJECT_TYPES                                                           \
  X(INTEGER_OBJECT)                                                                   \
  X(BOOLEAN_OBJECT)                                                                   \
  X(DONKEY_OBJECT)

#define X(name) name,
typedef enum { OBJECT_TYPES } ObjectType;
#undef X

#define X(name) (#name),
char *_ObjectToString[] = {OBJECT_TYPES};
#undef X
#define ObjectToString(TYPE) _ObjectToString[TYPE]

typedef struct {
  I64 value;
} ObjectInteger;

typedef struct {
  bool value;
} ObjectBoolean;

typedef String ObjectDonkey;

ObjectDonkey donkey_panic =
    (ObjectDonkey){.str = "(donkey)", .len = 8, .cap = 8};

typedef struct {
  ObjectType type;
  union {
    ObjectInteger integer;
    ObjectBoolean boolean;
    ObjectDonkey donkey; // this is null btw
  };
} Object;

Object eval_evaluate_program(Arena *arena, Program program);
Object eval_evaluate_node(Arena *, Node *);
Object eval_evaluate_expression(Arena *, ExpressionStatement);
Object test_eval(char *);
String object_to_string(Arena *, Object object);

#endif // !_OBJECT_H
