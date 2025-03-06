#include "object.h"

ObjectDonkey donkey_panic =
    (ObjectDonkey){.str = "(donkey)", .len = 8, .cap = 8};
Object DONKEY_PANIC_OBJECT =
    (Object){.type = DONKEY_OBJECT, .donkey = (ObjectDonkey){.str = "(donkey)", .len = 8, .cap = 8}};
Object TRUE_OBJECT = (Object){.type = BOOLEAN_OBJECT, .boolean.value = true};
Object FALSE_OBJECT = (Object){.type = BOOLEAN_OBJECT, .boolean.value = false};
// TODO: check why src/testing/../object/object.c:10:72: error: initializer element is not a compile-time constant 10 | Object TRUE_OBJECT = (Object){.type = INTEGER_OBJECT, .integer.value = popo};
/* int popo = 123;*/
/* Object TRUE_OBJECT = (Object){.type = INTEGER_OBJECT, .integer.value = popo};*/

Object eval_evaluate_expression(Arena *arena,
                                ExpressionStatement expression_statement) {

  Expression *expression = expression_statement.expression_value;
  Object evaluated_object =
      (Object){.type = DONKEY_OBJECT, .donkey = donkey_panic};

  switch (expression->type) {
  case INTEGER_LIT_EXP:
    //
    {

      evaluated_object.type = INTEGER_OBJECT;
      evaluated_object.integer.value = expression->integer_literal.value;
      break;
    }
  case BOOLEAN_EXP:
    //
    {
      Boolean boolean = expression->boolean;
      if (boolean.value == true) {
        evaluated_object = TRUE_OBJECT;
      } else {
        evaluated_object = FALSE_OBJECT;
      }
      break;
    }
  default:
    break;
  }
  return evaluated_object;
}

Object eval_evaluate_node(Arena *arena, Node *node) {

  Object evaluated_object =
      (Object){.type = DONKEY_OBJECT, .donkey = donkey_panic};
  switch (node->type) {
  case EXPRESSION_STATEMENT:
    evaluated_object =
        eval_evaluate_expression(arena, node->expression_statement);
    break;
  default:
    printf("todo\n");
    break;
  }
  return evaluated_object;
}

Object eval_evaluate_program(Arena *arena, Program program) {
  Object evaluated_object =
      (Object){.type = DONKEY_OBJECT, .donkey = donkey_panic};
  for (I64 i = 0; i < len(program.statements); i++) {
    Node *node = &program.statements[i];
    evaluated_object = eval_evaluate_node(arena, node);
  }
  return evaluated_object;
}

String object_to_string(Arena *arena, Object object) {
  switch (object.type) {
  case INTEGER_OBJECT:
    //
    {
      return arena_string_fmt(arena, "%d", object.integer.value);
      break;
    }

  case BOOLEAN_OBJECT:
    //
    {
      return arena_string_fmt(arena, "%b", object.boolean.value);
      break;
    }

  case DONKEY_OBJECT:
    //
    {
      return arena_string_fmt(arena, "%S", object.donkey);
      break;
    }
  }
}
