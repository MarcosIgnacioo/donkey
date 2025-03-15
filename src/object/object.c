#include "object.h"
#include "../arena_strings.c"
#include "../ram/ram.c"
// remove the arena passing all around is annoyinnn and also i dont do any
// allocations i think

// TODO: check why src/testing/../object/object.c:10:72: error: initializer
// element is not a compile-time constant 10 | Object TRUE_OBJECT =
// (Object){.type = INTEGER_OBJECT, .integer.value = popo};
/* int popo = 123;*/
/* Object TRUE_OBJECT = (Object){.type = INTEGER_OBJECT, .integer.value =
 * popo};*/

Object eval_evaluate_expression(Arena *arena, Enviroment *env,
                                Expression *expression) {
  Object evaluated_object = DONKEY_PANIC_OBJECT;
  switch (expression->type) {
  case IDENTIFIER_EXP:
    //
    {
      Identifier identifier = expression->identifier;
      evaluated_object = env_get_object(arena, env, identifier.value);
      break;
    }
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
      Object right = eval_evaluate_expression(arena, env, prefix.right);
      evaluated_object = eval_prefix_expression(arena, prefix.operator, right);
      break;
    }
  case INFIX_EXP:
    //
    {
      InfixExpression infix = expression->infix;
      Object left = eval_evaluate_expression(arena, env, infix.left);
      Object right = eval_evaluate_expression(arena, env, infix.right);
      evaluated_object =
          eval_infix_expression(arena, left, infix.operator, right);
      break;
    }
  case IF_EXP:
    //
    {
      IfExpression if_expression = expression->if_expression;
      Object condition =
          eval_evaluate_expression(arena, env, if_expression.condition);
      BlockStatement consequence = if_expression.consequence;
      BlockStatement alternative = if_expression.alternative;
      /*Enviroment *if_env = arena_alloc(arena, sizeof(Enviroment));*/
      Enviroment if_env = {0};
      env_clone(arena, &if_env, env);
      evaluated_object = eval_if_expression(arena, &if_env, condition,
                                            consequence, alternative);
      break;
    }
  case FUNCTION_LITERAL_EXP:
    //
    {
      FunctionLiteral function = expression->function_literal;
      evaluated_object = eval_fn_expression(arena, env, function.name.value,
                                            function.body, function.parameters);
      break;
    }
  case FUNCTION_CALL_EXP:
    //
    {
      FunctionCallExpression function_call = expression->function_call;
      Object fn_object =
          eval_evaluate_expression(arena, env, function_call.function);
      if (fn_object.type == ERROR_OBJECT) {
        return fn_object;
      }
      Object *arguments =
          eval_evaluate_expressions(arena, env, function_call.arguments);
      // we check the first element of the array
      // if it is an error well we then return it :D
      if (arguments->type == ERROR_OBJECT) {
        return *arguments;
      }
      ObjectFunction function = fn_object.function;
      evaluated_object = eval_fn_call_expression( //
          arena,                                  //
          env,                                    //
          function,                               //
          arguments                               //
      );
      // here tmp arena would be super cool for storing the arguments
      // cause after evaluating the fn_call_expression we dont really
      // need them anymore so freeing that memory would be pretty cool
      break;
    }
  default:
    break;
  }
  return evaluated_object;
}

String error_stringify_object_type(Object object) {
  String str_type = string(ObjectToString(object.type));
  string_chop_until(&str_type, '_');
  return str_type;
}

Object new_error(Arena *arena, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  String error_message = vargs_arena_string_fmt(arena, fmt, args);
  va_end(args);

  Object error_type_mismatch = _new_error(error_message);
  return error_type_mismatch;
}

Object eval_prefix_expression(Arena *arena, String operator, Object right) {
  if (right.type != BOOLEAN_OBJECT && right.type != INTEGER_OBJECT) {
    String right_type = error_stringify_object_type(right);
    Object error_object =
        new_error(arena, "%S != BOOLEAN or INTEGER ", right_type);
    return error_object;
  }

  if (right.type == INTEGER_OBJECT && string_equals(operator, MINUS_STRING)) {
    right.integer.value = -right.integer.value;
    return right;
  } else if (right.type == BOOLEAN_OBJECT &&
             string_equals(operator, BANG_STRING)) {
    right.boolean.value = !right.boolean.value;
    return right;
  }

  String right_type = error_stringify_object_type(right);
  Object error_object =
      new_error(arena, "unknown operator: %S%S", operator, right_type);

  return error_object;
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
Object eval_infix_expression(Arena *arena, Object left, String operator,
                             Object right) {
  // pass this to
  if (right.type == INTEGER_OBJECT && left.type == INTEGER_OBJECT) {
    return eval_integer_infix_expression(arena, left, operator, right);
  }

  if (right.type == BOOLEAN_OBJECT && left.type == BOOLEAN_OBJECT) {
    return eval_bool_infix_expression(arena, left, operator, right);
  }

  Object error_object;

  if (left.type != right.type) {
    String left_type = error_stringify_object_type(left);
    String right_type = error_stringify_object_type(right);
    error_object = new_error(arena, "type mismatch: %S %s %S",
                             left_type, operator.str, right_type);
    return error_object;
  } else {
    String left_type = error_stringify_object_type(left);
    String right_type = error_stringify_object_type(right);
    error_object = new_error(arena, "unknown operator: %S %s %S",
                             left_type, operator.str, right_type);
  }

  return error_object;
}

bool is_truthy(Object condition) {

  if (condition.type == NIL_OBJECT) {
    return false;
  }

  if (condition.type == BOOLEAN_OBJECT) {
    bool bool_value = condition.boolean.value;
    return bool_value;
  }

  // lets see if 0 evaluates to false which i dont think so
  if (condition.type == INTEGER_OBJECT) {
    I64 value = condition.integer.value;
    if (value != 0) {
      return true;
    } else {
      return false;
    }
  }

  return true;
}

Object eval_if_expression(Arena *arena, Enviroment *env, Object condition,
                          BlockStatement consequence,
                          BlockStatement alternative) {
  if (is_truthy(condition)) {
    return eval_evaluate_block_statements(arena, env, consequence);
  } else {
    return eval_evaluate_block_statements(arena, env, alternative);
  }
}

Object eval_fn_expression(Arena *arena,        //
                          Enviroment *env,     //
                          String name,         //
                          BlockStatement body, //
                          Identifier *parameters) {
  /*Enviroment *fn_env = arena_alloc(arena, sizeof(Enviroment));*/
  Object evaluated_object;
  ObjectFunction *function = &evaluated_object.function;
  function->parameters = parameters;
  function->body = body;
  /*function->env = fn_env;*/
  evaluated_object.type = FUNCTION_OBJECT;
  if (name.len) {
    env_insert_object(arena, env, name, evaluated_object);
    evaluated_object.function.name = name;
  } else {
    evaluated_object.function.name = (String){.str = "", .len = 0, .cap = 0};
  }
  return evaluated_object;
}

Object *eval_evaluate_expressions(Arena *arena, Enviroment *env,
                                  Expression **expressions) {
  Object *resulting_objects = arena_array(arena, Object);

  for (int i = 0; i < len(expressions); i++) {
    Expression *expression = expressions[i];
    Object evaluated_object = eval_evaluate_expression(arena, env, expression);
    if (evaluated_object.type == ERROR_OBJECT) {
      reset(resulting_objects);
      append(resulting_objects, evaluated_object);
      break;
    }
    append(resulting_objects, evaluated_object);
  }

  return resulting_objects;
}

Object eval_fn_call_expression(Arena *arena,            //
                               Enviroment *env,         //
                               ObjectFunction function, //
                               Object *arguments) {
  // store the function enviroment pointer
  Enviroment *fn_env = arena_alloc(arena, sizeof(Enviroment));
  function.env = fn_env;
  env_clone(arena, fn_env, env);
  BlockStatement fn_body = function.body;
  Identifier *parameters = function.parameters;

  for (int i = 0; i < len(parameters); i++) {
    Object arg = arguments[i];
    String parameter_name = parameters[i].value;
    env_insert_object(arena, fn_env, parameter_name, arg);
  }

  // we should free the enviromentttttt!!!
  Object returning_object =
      eval_evaluate_block_statements(arena, fn_env, fn_body);
  return returning_object;
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

Object eval_integer_infix_expression(Arena *arena, Object left, String operator,
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
        Object error_object =
            new_error(arena, "expected: = \ngot:%S", operator);
        return error_object;
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
        Object error_object =
            new_error(arena, "expected: = \ngot:%S", operator);
        return error_object;
      }
    }
  default:
    //
    {
      String left_type = error_stringify_object_type(left);
      String right_type = error_stringify_object_type(right);
      Object error_object = new_error(arena, "unknown operator: %S %S %S",
                                      left_type, operator, right_type);
      return error_object;
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

Object eval_bool_infix_expression(Arena *arena, Object left, String operator,
                                  Object right) {
  Object product_object = (Object){0};
  I64 result = 0;
  OperationFunction operation;

  switch (*operator.str) {
  case '>':
    //
    {
      if (operator.len> 1 && operator.str[1] == '=') {
        operation = &greater_equal_than;
        break;
      } else {
        operation = &greater_than;
        break;
      }
    }
  case '<':
    // try <+
    {
      if (operator.len> 1 && operator.str[1] == '=') {
        operation = &less_equal_than;
        break;
      } else {
        operation = &less_than;
        break;
      }
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
      String left_type = error_stringify_object_type(left);
      String right_type = error_stringify_object_type(right);
      Object error_object = new_error(arena, "unknown operator: %S %S %S",
                                      left_type, operator, right_type);
      return error_object;
    }
  }

  result = operation(left.boolean.value, right.boolean.value);
  product_object.boolean.value = result;
  product_object.type = BOOLEAN_OBJECT;

  return product_object;
}

// yeah basically refactor this whole thing
// and watch in the debugger what happens to 10
// which is probably just that when evaluating the content
// of a nested if the 10 turns itself into from EVAL_RETURN to INTEGER_OBJECT
Object eval_evaluate_node(Arena *arena, Enviroment *env, Node *node) {
  Object evaluated_object = DONKEY_PANIC_OBJECT;
  if (!node) {
    return evaluated_object;
  }

  switch (node->type) {
  case EXPRESSION_STATEMENT:
    evaluated_object = eval_evaluate_expression(
        arena, env, node->expression_statement.expression_value);
    break;
  case RETURN_STATEMENT:
    evaluated_object = eval_evaluate_expression(
        arena, env, node->return_statement.expression_value);
    evaluated_object.eval_type = EVAL_RETURN;
    break;
  case LET_STATEMENT: {
    LetStatement let_statement = node->let_statement;
    Identifier identifier = let_statement.name;
    Expression *expression_value = let_statement.expression_value;
    evaluated_object = eval_evaluate_expression(arena, env, expression_value);
    if (evaluated_object.type == ERROR_OBJECT) {
      return evaluated_object;
    }
    env_insert_object(arena, env, identifier.value, evaluated_object);
    break;
  }
  default:
    printf("todo\n");
    break;
  }
  return evaluated_object;
}

Object eval_evaluate_program(Arena *arena, Enviroment *env, Program program) {
  Object evaluated_object = DONKEY_PANIC_OBJECT;
  // i dont think i need to pass the env as a parameter
  // here because this function lives the whole evaluation
  // lifetime so yeah

  for (I64 i = 0; i < len(program.statements); i++) {
    Node *node = &program.statements[i];
    evaluated_object = eval_evaluate_node(arena, env, node);
    if (evaluated_object.eval_type == EVAL_RETURN ||
        evaluated_object.eval_type == EVAL_ERROR) {
      break;
    }
  }

  return evaluated_object;
}

Object eval_evaluate_block_statements(Arena *arena, Enviroment *env,
                                      BlockStatement block_statement) {
  if (!block_statement.statements) {
    return DONKEY_PANIC_OBJECT;
  }

  Object evaluated_object = DONKEY_PANIC_OBJECT;

  for (I64 i = 0; i < len(block_statement.statements); i++) {
    Node *node = &block_statement.statements[i];
    evaluated_object = eval_evaluate_node(arena, env, node);
    if (evaluated_object.eval_type == EVAL_RETURN ||
        evaluated_object.eval_type == EVAL_ERROR) {
      break;
    }
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
  case FUNCTION_OBJECT:
    //
    {
      ObjectFunction fn = object.function;
      String parameters = arena_join_identifier_array(arena, fn.parameters);
      String body = stringify_statements(arena, fn.body.statements);
      String name = fn.name;
      if (true) {
      }
      return arena_string_fmt(arena, "fn %S(%S) { %S }", name, parameters,
                              body);
      break;
    }
  case ERROR_OBJECT:
    //
    {
      return arena_string_fmt(arena, "[ERROR]: %S", object.error.value);
      break;
    }
  default:
    //
    {
      return arena_string_fmt(arena, "not stringrified amix", object.donkey);
      break;
    }
  }
}
