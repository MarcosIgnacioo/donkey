#include "object.h"

ObjectDonkey donkey_panic =
    (ObjectDonkey){.str = "(donkey)", .len = 8, .cap = 8};
Object DONKEY_PANIC_OBJECT =
    (Object){.type = DONKEY_OBJECT,
             .donkey = (ObjectDonkey){.str = "(donkey)", .len = 8, .cap = 8}};
Object TRUE_OBJECT = (Object){.type = BOOLEAN_OBJECT, .boolean.value = true};
Object FALSE_OBJECT = (Object){.type = BOOLEAN_OBJECT, .boolean.value = false};
String BANG_STRING = (String){.str = "!", .len = 1, .cap = 1};
String MINUS_STRING = (String){.str = "-", .len = 1, .cap = 1};
// TODO: check why src/testing/../object/object.c:10:72: error: initializer
// element is not a compile-time constant 10 | Object TRUE_OBJECT =
// (Object){.type = INTEGER_OBJECT, .integer.value = popo};
/* int popo = 123;*/
/* Object TRUE_OBJECT = (Object){.type = INTEGER_OBJECT, .integer.value =
 * popo};*/

Object eval_evaluate_expression(Arena *arena, Expression *expression) {

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
  case PREFIX_EXP:
    //
    {
      PrefixExpression prefix = expression->prefix;
      Object right = eval_evaluate_expression(arena, prefix.right);
      evaluated_object = eval_prefix_expression(prefix.operator, right);
      break;
    }
  case INFIX_EXP:
    //
    {
      InfixExpression infix = expression->infix;
      Object left = eval_evaluate_expression(arena, infix.left);
      Object right = eval_evaluate_expression(arena, infix.right);
      evaluated_object = eval_infix_expression(left, infix.operator, right);
      break;
    }
  default:
    break;
  }
  return evaluated_object;
}

Object eval_prefix_expression(String operator, Object right) {
  if (right.type != INTEGER_OBJECT && right.type != BOOLEAN_OBJECT) {
    return DONKEY_PANIC_OBJECT;
  }
  if (string_equals(operator, MINUS_STRING)) {
    right.integer.value = -right.integer.value;
  } else if (string_equals(operator, BANG_STRING)) {
    right.boolean.value = !right.boolean.value;
  }
  return right;
}

String ADD_STRING = (String){.str = "+", .len = 1, .cap = 1};
String MULTIPLY_STRING = (String){.str = "*", .len = 1, .cap = 1};
String DIVISION_STRING = (String){.str = "/", .len = 1, .cap = 1};

String GREATER_THAN_STRING = (String){.str = ">", .len = 1, .cap = 1};
String LESS_THAN_STRING = (String){.str = "<", .len = 1, .cap = 1};
String GREATER_EQUAL_STRING = (String){.str = ">=", .len = 2, .cap = 2};
String LESS_EQUAL_STRING = (String){.str = "<=", .len = 2, .cap = 2};
String EQUAL_STRING = (String){.str = "==", .len = 2, .cap = 2};
String NOT_EQUAL_STRING = (String){.str = "!=", .len = 2, .cap = 2};

// this madness is because we lose the operator as a token :sob:
// and because in c we dont have switch for something else than ints!
// it could be possible to create a custom switch tho so thats left to my
// spare time
Object eval_infix_expression(Object left, String operator, Object right) {
  if (right.type != INTEGER_OBJECT && left.type != INTEGER_OBJECT) {
    return DONKEY_PANIC_OBJECT;
  }
  ObjectInteger left_int = left.integer;
  ObjectInteger right_int = right.integer;
  Object product_object = DONKEY_PANIC_OBJECT;
  I64 result = 0;

  if (string_equals(operator, ADD_STRING)) {
    result = left_int.value + right_int.value;
    product_object.type = INTEGER_OBJECT;
    product_object.integer.value = result;
    return product_object;
  } //
  else if (string_equals(operator, MINUS_STRING)) {
    result = left_int.value - right_int.value;
    product_object.type = INTEGER_OBJECT;
    product_object.integer.value = result;
    return product_object;
  } //
  else if (string_equals(operator, DIVISION_STRING)) {
    result = left_int.value / right_int.value;
    product_object.type = INTEGER_OBJECT;
    product_object.integer.value = result;
    return product_object;
  } //
  else if (string_equals(operator, MULTIPLY_STRING)) {
    result = left_int.value * right_int.value;
    product_object.type = INTEGER_OBJECT;
    product_object.integer.value = result;
    return product_object;
  }

  if (string_equals(operator, GREATER_THAN_STRING)) {
    result = left_int.value > right_int.value;
    product_object.boolean.value = (bool)result;
    product_object.type = BOOLEAN_OBJECT;
    return product_object;
  } //
  else if (string_equals(operator, LESS_THAN_STRING)) {
    result = left_int.value < right_int.value;
    product_object.boolean.value = (bool)result;
    product_object.type = BOOLEAN_OBJECT;
    return product_object;
  } //
  else if (string_equals(operator, GREATER_EQUAL_STRING)) {
    result = left_int.value >= right_int.value;
    product_object.boolean.value = (bool)result;
    product_object.type = BOOLEAN_OBJECT;
    return product_object;
  } //
  else if (string_equals(operator, LESS_EQUAL_STRING)) {
    result = left_int.value <= right_int.value;
    product_object.boolean.value = (bool)result;
    product_object.type = BOOLEAN_OBJECT;
    return product_object;
  } //
  else if (string_equals(operator, EQUAL_STRING)) {
    result = left_int.value == right_int.value;
    product_object.boolean.value = (bool)result;
    product_object.type = BOOLEAN_OBJECT;
    return product_object;
  } //
  else if (string_equals(operator, NOT_EQUAL_STRING)) {
    result = left_int.value != right_int.value;
    product_object.boolean.value = (bool)result;
    product_object.type = BOOLEAN_OBJECT;
    return product_object;
  } //
  // this one fails (5 > 5 == true) != false
  else {
    return DONKEY_PANIC_OBJECT;
  }
}

Object eval_evaluate_node(Arena *arena, Node *node) {

  Object evaluated_object =
      (Object){.type = DONKEY_OBJECT, .donkey = donkey_panic};
  switch (node->type) {
  case EXPRESSION_STATEMENT:
    evaluated_object = eval_evaluate_expression(
        arena, node->expression_statement.expression_value);
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
