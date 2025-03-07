#include "object.h"

ObjectDonkey donkey_panic =
    (ObjectDonkey){.str = "(donkey)", .len = 8, .cap = 8};
Object DONKEY_PANIC_OBJECT =
    (Object){.type = NIL_OBJECT,
             .donkey = (ObjectDonkey){.str = "(null)", .len = 8, .cap = 8}};
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

  Object evaluated_object = DONKEY_PANIC_OBJECT;

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

I64 donkey_operation(I64 a, I64 b) {
  (void)a;
  (void)b;
  printf("panic at the operation!\n");
  return -327;
}

I64 addition(I64 a, I64 b) { return a + b; }

I64 substraction(I64 a, I64 b) { return a - b; }

I64 multiplication(I64 a, I64 b) { return a * b; }

I64 division(I64 a, I64 b) { return a / b; }

I64 greater_than(I64 a, I64 b) { return a > b; }

I64 less_than(I64 a, I64 b) { return a < b; }

I64 equal_to(I64 a, I64 b) { return a == b; }

I64 not_equal_to(I64 a, I64 b) { return a != b; }

I64 less_equal_than(I64 a, I64 b) { return a <= b; }

I64 greater_equal_than(I64 a, I64 b) { return a >= b; }

// this madness is because we lose the operator as a token :sob:
// and because in c we dont have switch for something else than ints!
// it could be possible to create a custom switch tho so thats left to my
// spare time
Object eval_infix_expression(Object left, String operator, Object right) {
  if (right.type == INTEGER_OBJECT && right.type == INTEGER_OBJECT) {
    return eval_integer_infix_expression(left, operator, right);
  }
  if (right.type == BOOLEAN_OBJECT && right.type == BOOLEAN_OBJECT) {
    return eval_bool_infix_expression(left, operator, right);
  }
  return DONKEY_PANIC_OBJECT;
}

Object c_boolean_to_donkey_boolean(bool boolean) {
  Object product_object = {
      .type = BOOLEAN_OBJECT,
      .boolean.value = boolean,
  };
  return product_object;
}

bool is_arithmetic(String operator) {
  if (operator.len<1) {
    printf("bro how did u even manage to do this\n");
    exit(1);
  }
  char op = operator.str[0];
  return op == '+' || op == '-' || op == '*' || op == '/';
}

Object eval_integer_infix_expression(Object left, String operator,
                                     Object right) {
  Object product_object = (Object){0};
  OperationFunction operation = NULL;
  ObjectType result_type = NIL_OBJECT;
  /* breaks with
   * 500 / 2 != 250*/

  I64 result = 0;

  switch (*operator.str) {
  case '+':
    //
    {
      // assert that
      operation = &addition;
      result_type = INTEGER_OBJECT;
      break;
    }
  case '-':
    //
    {
      operation = &substraction;
      result_type = INTEGER_OBJECT;
      break;
    }
  case '*':
    //
    {
      operation = &multiplication;
      result_type = INTEGER_OBJECT;
      break;
    }
  case '/':
    //
    {
      operation = &division;
      result_type = INTEGER_OBJECT;
      break;
    }
  case '>':
    //
    {
      result_type = BOOLEAN_OBJECT;
      if (operator.len> 1 && operator.str[1] == '=') {
        operation = &greater_equal_than;
      } else {
        operation = &greater_than;
      }
      break;
    }
  case '<':
    //
    {
      result_type = BOOLEAN_OBJECT;
      if (operator.len> 1 && operator.str[1] == '=') {
        operation = &less_equal_than;
      } else {
        operation = &less_than;
      }
      break;
    }
  case '!':
    //
    {
      if (operator.len> 1 && operator.str[1] == '=') {
        operation = &not_equal_to;
        result_type = BOOLEAN_OBJECT;
        break;
      } else {
        printfln("falling to default with this operator : %S \n", operator);
      }
    }
  case '=':
    //
    {
      if (operator.len> 1 && operator.str[1] == '=') {
        operation = &equal_to;
        result_type = BOOLEAN_OBJECT;
        break;
      } else {
        printfln("falling to default with this operator : %S \n", operator);
      }
    }
  default:
    //
    {
      return DONKEY_PANIC_OBJECT;
    }
  }

  result = operation(left.integer.value, right.integer.value);

  if (result_type == INTEGER_OBJECT) {
    product_object.integer.value = result;
  } else {
    product_object.boolean.value = result;
  }

  product_object.type = result_type;

  return product_object;
}

Object eval_bool_infix_expression(Object left, String operator, Object right) {
  Object product_object = (Object){0};
  I64 result = 0;
  OperationFunction operation;

  switch (*operator.str) {
  case '>':
    //
    {
      if (operator.len> 1 && operator.str[1] == '=') {
        operation = &greater_equal_than;
      } else {
        operation = &greater_than;
      }
      break;
    }
  case '<':
    //
    {
      if (operator.len> 1 && operator.str[1] == '=') {
        operation = &less_equal_than;
      } else {
        operation = &less_than;
      }
      break;
    }
  case '!':
    //
    {
      if (operator.len> 1 && operator.str[1] == '=') {
        operation = &not_equal_to;
        break;
      }
    }
    // this is just because i wanna make it "branchless" so i
    // use the fallthrought property in this two when conditions
    // are not met at this point because it will just return to DONKEY_OBJECT
    // in the default branch
    // probably really bad for making something good? because
    // it is less understandable
  case '=':
    //
    {
      if (operator.len> 1 && operator.str[1] == '=') {
        operation = &equal_to;
        break;
      }
    }
  default:
    //
    {
      return DONKEY_PANIC_OBJECT;
    }
  }

  result = operation(left.boolean.value, right.boolean.value);
  product_object.boolean.value = result;
  product_object.type = BOOLEAN_OBJECT;

  return product_object;
}

Object eval_evaluate_node(Arena *arena, Node *node) {
  Object evaluated_object = DONKEY_PANIC_OBJECT;
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
  Object evaluated_object = DONKEY_PANIC_OBJECT;
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

  default:
    //
    {
      return arena_string_fmt(arena, "%S", object.donkey);
      break;
    }
  }
}
