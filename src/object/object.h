#ifndef _OBJECT_H
#define _OBJECT_H
#include "../arena_strings.c"
#include "../parser/ast.c"
#include <stdbool.h>

// put donkey at first idk if this breaks stuff i dont think so
#define OBJECT_TYPES                                                           \
  X(NIL_OBJECT)                                                                \
  X(DONKEY_OBJECT)                                                             \
  X(INTEGER_OBJECT)                                                            \
  X(BOOLEAN_OBJECT)

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

typedef struct {
  ObjectType type;
  union {
    ObjectInteger integer;
    ObjectBoolean boolean;
    ObjectDonkey donkey; // this is null btw
  };
} Object;

typedef I64 (*OperationFunction)(I64, I64);

typedef struct {
  ObjectType type;
  OperationFunction operation;
} OperationFnAndType;

Object eval_evaluate_program(Arena *, Program);
Object eval_evaluate_block_statements(Arena *, BlockStatement);
Object eval_prefix_expression(String, Object);
Object eval_infix_expression(Object, String, Object);
Object eval_evaluate_node(Arena *, Node *);
Object eval_evaluate_expression(Arena *, Expression *);
Object eval_integer_infix_expression(Object, String, Object);
Object eval_bool_infix_expression(Object, String, Object);
Object eval_if_expression(Arena *, Object, BlockStatement, BlockStatement);
String object_to_string(Arena *, Object);
Object test_eval(char *);

#endif // !_OBJECT_H
