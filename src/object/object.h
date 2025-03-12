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
  X(BOOLEAN_OBJECT)                                                            \
  X(STRING_OBJECT)                                                             \
  X(IDENTIFIER_OBJECT)                                                         \
  X(ERROR_OBJECT)

#define X(name) name,
typedef enum { OBJECT_TYPES } ObjectType;
#undef X

#define X(name) (#name),
char *_ObjectToString[] = {OBJECT_TYPES};
#undef X
#define ObjectToString(TYPE) _ObjectToString[TYPE]

typedef struct Object Object;

typedef struct {
  I64 value;
} ObjectInteger;

typedef struct {
  bool value;
} ObjectBoolean;

typedef enum {
  INT_TYPE,
  STRING_TYPE,
} DataType;

typedef struct {
  DataType type;
  union {
    String string_value;
    I64 integer_value;
  };
} ObjectIdentifier;

typedef struct {
  String value;
} ObjectString;

typedef struct {
  String value;
} ObjectError;

typedef String ObjectDonkey;
typedef enum { EVAL_NIL, EVAL_OBJECT, EVAL_RETURN, EVAL_ERROR } EvalType;

struct Object {
  EvalType eval_type;
  ObjectType type;
  /*ObjectType type;*/
  union {
    ObjectInteger integer;
    ObjectBoolean boolean;
    ObjectIdentifier identifier;
    ObjectString string;
    ObjectError error;
    ObjectDonkey donkey; // this is null btw
  };
};

typedef I64 (*OperationFunction)(I64, I64);

typedef struct {
  ObjectType type;
  OperationFunction operation;
} OperationFnAndType;

#define _new_error(MSG)                                                        \
  (Object) { .eval_type = EVAL_ERROR, .type = ERROR_OBJECT, .error.value = MSG }

ObjectDonkey donkey_panic =
    (ObjectDonkey){.str = "(donkey)", .len = 8, .cap = 8};
Object DONKEY_PANIC_OBJECT =
    (Object){.eval_type = EVAL_OBJECT,
             .type = NIL_OBJECT,
             .donkey = (ObjectDonkey){.str = "(null)", .len = 8, .cap = 8}};
Object TRUE_OBJECT = (Object){.type = BOOLEAN_OBJECT, .boolean.value = true};
Object FALSE_OBJECT = (Object){.type = BOOLEAN_OBJECT, .boolean.value = false};
String BANG_STRING = (String){.str = "!", .len = 1, .cap = 1};
String MINUS_STRING = (String){.str = "-", .len = 1, .cap = 1};

Object eval_evaluate_program(Arena *, Program);
Object eval_evaluate_block_statements(Arena *, BlockStatement);
Object eval_prefix_expression(Arena *, String, Object);
Object eval_infix_expression(Arena *, Object, String, Object);
Object eval_evaluate_node(Arena *, Node *);
Object eval_evaluate_expression(Arena *, Expression *);
Object eval_integer_infix_expression(Arena *, Object, String, Object);
Object eval_bool_infix_expression(Arena *, Object, String, Object);
Object eval_if_expression(Arena *, Object, BlockStatement, BlockStatement);
String object_to_string(Arena *, Object);
Object test_eval(char *);

#endif // !_OBJECT_H
