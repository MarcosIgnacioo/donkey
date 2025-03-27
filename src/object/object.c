#include "object.h"
#include "../arena_strings.c"
#include "../env/env.c"
// remove the arena passing all around is annoyinnn and also i dont do any
// allocations i think

// TODO: check why src/testing/../object/object.c:10:72: error: initializer
// element is not a compile-time constant 10 | Object TRUE_OBJECT =
// (Object){.type = INTEGER_OBJECT, .integer.value = popo};
/* int popo = 123;*/
/* Object TRUE_OBJECT = (Object){.type = INTEGER_OBJECT, .integer.value =
 * popo};*/

Enviroment built_in_env = {0};

Object eval_evaluate_expression(Arena *arena, Enviroment *env,
                                Expression *expression) {
  Object evaluated_object = DONKEY_PANIC_OBJECT;
  switch (expression->type) {
  case IDENTIFIER_EXP:
    //
    {
      Identifier identifier = expression->identifier;
      evaluated_object = env_get_object(arena, env, identifier.value);
      if (evaluated_object.type == ERROR_OBJECT) {
        evaluated_object =
            env_get_object(arena, &built_in_env, identifier.value);
      }
      break;
    }
  case INTEGER_LIT_EXP:
    //
    {
      evaluated_object.type = INTEGER_OBJECT;
      evaluated_object.integer.value = expression->integer_literal.value;
      break;
    }
  case STRING_LIT_EXP:
    //
    {
      evaluated_object.type = STRING_OBJECT;
      evaluated_object.string.value = expression->string_literal.value;
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
      evaluated_object =
          eval_if_expression(arena, env, condition, consequence, alternative);
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
      Object fn_call_object =
          eval_evaluate_expression(arena, env, function_call.function);
      evaluated_object =
          eval_evaluate_fn_call(arena, env, function_call, fn_call_object);
      break;
    }
  case ARRAY_EXP:
    //
    {
      Array array_declaration = expression->array;
      evaluated_object = eval_evaluate_array(arena, env, array_declaration);
      break;
    }
  case HASH_MAP_EXP_:
    //
    {
      HashLiteral_ hash_map_declaration = expression->hash_literal_;
      evaluated_object =
          eval_evaluate_hash_map(arena, env, &hash_map_declaration);
      // cmabio nuevo
      break;
    }
  case KEY_HASH_MAP_EXP:
    //
    {
      KeyHash array_indexing = expression->key_hash;
      evaluated_object =
          eval_evaluate_index_hash_map(arena, env, array_indexing);
      break;
    }
  case INDEX_EXP:
    //
    {
      Index array_indexing = expression->index;
      evaluated_object = eval_evaluate_index_array(arena, env, array_indexing);
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

// expects dynamic array
String arena_join_object_array(Arena *arena, Object *objects) {
  String join = arena_new_empty_string_with_cap(arena, 512);
  arena_string_concat(arena, &join, string("["));
  for (I64 i = 0; i < len(objects); i++) {
    if (i > 0) {
      arena_string_concat(arena, &join, string(", "));
    }
    String obj = object_to_string(arena, objects[i]);
    string_concat(&join, obj);
  }
  arena_string_concat(arena, &join, string("]"));
  return join;
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

  if (right.type == STRING_OBJECT && left.type == STRING_OBJECT) {
    return eval_string_infix_expression(arena, left, operator, right);
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

Object _eval_fn_expression(Arena *arena,        //
                           Enviroment *env,     //
                           String name,         //
                           BlockStatement body, //
                           Identifier *parameters) {
  Enviroment *fn_env = arena_alloc(arena, sizeof(Enviroment));
  env_clone(arena, fn_env, env);
  Object evaluated_object;
  ObjectFunction *function = &evaluated_object.function;
  function->parameters = parameters;
  function->body = body;
  function->env = fn_env;
  evaluated_object.type = FUNCTION_OBJECT;
  if (name.len) {
    env_insert_object(arena, env, name, evaluated_object);
    evaluated_object.function.name = name;
  } else {
    evaluated_object.function.name = (String){.str = "", .len = 0, .cap = 0};
  }
  return evaluated_object;
}

Object eval_fn_expression(Arena *arena,        //
                          Enviroment *env,     //
                          String name,         //
                          BlockStatement body, //
                          Identifier *parameters) {
  Object evaluated_object;
  ObjectFunction *function = &evaluated_object.function;
  function->parameters = parameters;
  function->body = body;
  function->env = env;
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

// the expressions here should all BE InfixExpression's!
HashTable *eval_evaluate_key_values_expression_list(Arena *arena,
                                                    Enviroment *env,
                                                    Expression **expressions) {
  HashTable *resulting_objects = arena_alloc(arena, sizeof(HashTable));

  for (size_t i = 0; i < len(expressions); i++) {
    Expression *expression = expressions[i];
    InfixExpression key_value = expression->infix;
    String key_str = stringify_expression(arena, NULL, key_value.left);
    Object value = eval_evaluate_expression(arena, env, key_value.right);
    insert_object(arena, resulting_objects, key_str, value);
  }

  return resulting_objects;
}

Object *eval_evaluate_key_values(Arena *arena, Enviroment *env,
                                 HashMapKeyValue **key_values) {
  Object *resulting_objects = arena_array(arena, Object);

  for (int i = 0; i < len(key_values); i++) {
    HashMapKeyValue *kv = key_values[i];
    Expression *key = kv->key;
    Expression *value = kv->value;
    Object eval_key = eval_evaluate_expression(arena, env, key);
    Object eval_value = eval_evaluate_expression(arena, env, value);
    if (eval_key.type == ERROR_OBJECT) {
      reset(resulting_objects);
      append(resulting_objects, eval_key);
      break;
    }
    if (eval_value.type == ERROR_OBJECT) {
      reset(resulting_objects);
      append(resulting_objects, eval_value);
      break;
    }
  }

  return resulting_objects;
}

void eval_expand_function_enviroment(Arena *arena, Enviroment *env,
                                     Identifier *parameters,
                                     Object *arguments) {
  for (int i = 0; i < len(parameters); i++) {
    Object arg = arguments[i];
    String parameter_name = parameters[i].value;
    env_insert_object(arena, env, parameter_name, arg);
  }
}

void eval_unwrap_function_return(Object *returning_object) {
  if (returning_object->eval_type == EVAL_RETURN) {
    returning_object->eval_type = EVAL_OBJECT;
  }
}

Object eval_evaluate_fn_call(Arena *arena,    //
                             Enviroment *env, //
                             FunctionCallExpression function_call,
                             Object fn_call_object) {
  Object evaluated_object = {0};

  if (fn_call_object.type == ERROR_OBJECT) {
    return fn_call_object;
  }

  Object *arguments =
      eval_evaluate_expressions(arena, env, function_call.arguments);

  // we check the first element of the array
  // if it is an error well we then return it :D
  if (arguments->type == ERROR_OBJECT) {
    return *arguments;
  }

  if (fn_call_object.type == BUILT_IN_OBJECT) {
    ObjectBuiltIn function = fn_call_object.built_in;
    evaluated_object = function.value(arena, arguments);
  } else if (fn_call_object.type == FUNCTION_OBJECT) {
    ObjectFunction function = fn_call_object.function;
    evaluated_object = eval_fn_call_expression( //
        arena,                                  //
        env,                                    //
        function,                               //
        arguments                               //
    );
  }

  // here tmp arena would be super cool for storing the arguments
  // cause after evaluating the fn_call_expression we dont really
  // need them anymore so freeing that memory would be pretty cool
  return evaluated_object;
}

Object eval_evaluate_array(Arena *arena, Enviroment *env,
                           Array array_declaration) {
  Object evaluated_object = {0};
  Object *members =
      eval_evaluate_expressions(arena, env, array_declaration.value);
  if (len(members) == 1 && members[0].type == ERROR_OBJECT) {
    return members[0];
  }

  evaluated_object.type = ARRAY_OBJECT;
  evaluated_object.array.value = members;
  return evaluated_object;
}

#define is_error(object) object.type == ERROR_OBJECT

#include "./object_hashmap.c"

Object eval_evaluate_hash_map(Arena *arena, Enviroment *env,
                              HashLiteral_ *hash_map_declaration) {
  Object evaluated_object = {0};
  Expression **expressions = hash_map_declaration->value;
  HashTable *members = arena_alloc(arena, sizeof(HashTable));

  for (size_t i = 0; i < len(expressions); i++) {
    Expression *expression = expressions[i];
    InfixExpression key_value = expression->infix;
    Object key = eval_evaluate_expression(arena, env, key_value.left);
    Object value = eval_evaluate_expression(arena, env, key_value.right);

    if (is_error(key)) {
      return key;
      break;
    }

    if (is_error(value)) {
      return value;
      break;
    }

    object_hash_map_insert(arena, members, key, value);
  }

  evaluated_object.type = HASH_MAP_OBJECT;
  evaluated_object.hash_map.value = members;
  return evaluated_object;
}

Object eval_evaluate_index_array(Arena *arena, Enviroment *env,
                                 Index indexing) {
  Object evaluated_object = {0};
  Object array = eval_evaluate_expression(arena, env, indexing.data);
  if (array.type != ARRAY_OBJECT) {
    String not_matching_type = error_stringify_object_type(array);
    return new_error(arena, "identifier is not an array, got : %S",
                     not_matching_type);
  }
  Object index = eval_evaluate_expression(arena, env, indexing.index);
  if (index.type != INTEGER_OBJECT) {
    String not_matching_type = error_stringify_object_type(index);
    return new_error(arena, "index is not an int, got : %S", not_matching_type);
  }
  I64 user_index = index.integer.value;
  Object *user_array = array.array.value;
  // this worked even with negative numbers and "detecting" them as errors
  // because a -1 is 0xFFFFFF which is promoted
  // to an integer, and then is a big nummber cause all bits are on
  // which most likely will always be greater than
  // the len so it evaluated to true and returned the null object
  // -------------------vvvvvvvvvvvvvvvvvvvvvvvvvvvv
  if (0 > user_index || user_index >= len(user_array)) {
    return DONKEY_PANIC_OBJECT;
  }
  evaluated_object = user_array[user_index];
  return evaluated_object;
}

Object eval_evaluate_index_hash_map(Arena *arena, Enviroment *env,
                                    KeyHash hash_indexing) {
  Object evaluated_object = {0};
  Object evaluated_hash_map =
      eval_evaluate_hash_map(arena, env, hash_indexing.hash_map);
  HashTable *table = evaluated_hash_map.hash_map.value;
  Object index = eval_evaluate_expression(arena, env, hash_indexing.index);
  evaluated_object = object_hash_map_get_item(table, index);
  return evaluated_object;
}

Object _eval_fn_call_expression(Arena *arena,            //
                                Enviroment *env,         //
                                ObjectFunction function, //
                                Object *arguments) {
  // the enviroment doesnt get stored in inner functions
  // aka closures dont work
  // which makes me sad
  // but im hungri
  // the easiest way to fix this would be just to make
  // everything return an object pointer bug ahhhhghghhhhh
  // i dont really wanna do that, the other way around that
  // would be to return an Object pointer in the get_from_env
  // because then it makes sense we wanna manipulate that env
  Enviroment *fn_env = function.env;
  fn_env->outer_memory = env;
  BlockStatement fn_body = function.body;
  Identifier *parameters = function.parameters;

  eval_expand_function_enviroment(arena, fn_env, parameters, arguments);

  Object returning_object =
      eval_evaluate_block_statements(arena, fn_env, fn_body);
  eval_unwrap_function_return(&returning_object);
  return returning_object;
}

Object eval_fn_call_expression(Arena *arena,            //
                               Enviroment *env,         //
                               ObjectFunction function, //
                               Object *arguments) {
  Enviroment *fn_env = arena_alloc(arena, sizeof(Enviroment));
  fn_env->outer_memory = function.env;
  BlockStatement fn_body = function.body;
  Identifier *parameters = function.parameters;

  eval_expand_function_enviroment(arena, fn_env, parameters, arguments);

  Object returning_object =
      eval_evaluate_block_statements(arena, fn_env, fn_body);
  eval_unwrap_function_return(&returning_object);
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
    // why do we allow this kind of crazyness
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

Object eval_string_infix_expression(Arena *arena, Object left, String operator,
                                    Object right) {
  Object product_object = (Object){0};
  String left_string = left.string.value;
  String right_string = right.string.value;
  switch (*operator.str) {
  case '+':
    //
    {
      ObjectString concat_string = {0};
      concat_string.value = arena_new_empty_string_with_cap(
          arena, left_string.len + right_string.len);
      string_concat(&concat_string.value, left_string);
      string_concat(&concat_string.value, right_string);
      product_object.string = concat_string;
      product_object.type = STRING_OBJECT;
      break;
    }
  case '-':
    //
    {
      ObjectString substract_string = {0};
      substract_string.value =
          arena_string_substract(arena, left_string, right_string);
      product_object.string = substract_string;
      product_object.type = STRING_OBJECT;
      break;
    }
  case '!':
    //
    {
      if (operator.len> 1 && operator.str[1] == '=') {
        ObjectBoolean not_equals = {0};
        not_equals.value = !string_equals(left_string, right_string);
        product_object.type = BOOLEAN_OBJECT;
        product_object.boolean = not_equals;
        break;
      }
    }
  case '=':
    //
    {
      if (operator.len> 1 && operator.str[1] == '=') {
        ObjectBoolean equals = {0};
        equals.value = string_equals(left_string, right_string);
        product_object.type = BOOLEAN_OBJECT;
        product_object.boolean = equals;
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

// we must pass an array of objects here always jjijiijijij YAY
Object _len(Arena *arena, Object *args) {
  if (len(args) > 1) {
    return new_error(arena, "wrong number of arguments. got=%d, want=1",
                     len(args));
  }
  Object donkey_arg = args[0];
  ObjectInteger length = (ObjectInteger){.value = 0};
  switch (donkey_arg.type) {
  case ARRAY_OBJECT:
    //
    {
      ObjectArray donkey_array = donkey_arg.array;
      length.value = len(donkey_array.value);
      break;
    }
  case STRING_OBJECT:
    //
    {
      ObjectString donkey_string = donkey_arg.string;
      length.value = (int)donkey_string.value.len;
      break;
    }
  default: {
    /*length.value = 0;*/
    return new_error(arena, "argument to `len` not supported, got %S",
                     error_stringify_object_type(donkey_arg));
  }
  }
  Object length_object = (Object){
      .type = INTEGER_OBJECT,   //
      .eval_type = EVAL_OBJECT, //
      .integer = length         //
  };
  return length_object;
}

Object _first(Arena *arena, Object *args) {
  if (len(args) > 1) {
    return new_error(arena, "wrong number of arguments. got=%d, want=1",
                     len(args));
  }

  Object donkey_arg = args[0];
  Object return_object = (Object){
      .eval_type = EVAL_OBJECT, //
  };

  switch (donkey_arg.type) {
  case ARRAY_OBJECT:
    //
    {
      ObjectArray donkey_array = donkey_arg.array;
      Object *array = donkey_array.value;
      if (len(array) == 0) {
        return DONKEY_PANIC_OBJECT;
      }
      // imagine this sceneario
      // first([])
      // here we might think well we should check if the
      // array is empty no? which yeah that should be the cool
      // and based option BUT, there is a little neat trick,
      // the default NIL values in enums, why?
      // well because when we evaluate a list of expressions
      // we are passed the expressions in an array from the parser
      // which are just an empty array, so we never evaluate any
      // expression cause there arent any expressions!!! so cool rite
      // well there is more to it, when doing an arena allocation we
      // always zero the memory, which makes all the structs and objects
      // we have allocated from it have default nil values
      // which means that the first element in the array we got returned from
      // the expressions the parser got (just an empty array) has nil values
      // which welllll we can just return and its so amazing and oocoool
      // aasdsfjaskdf
      /*if (!len(array)) {*/
      /*  return DONKEY_PANIC_OBJECT;*/
      /*}*/
      return_object = array[0];
      break;
    }
  default: {
    return new_error(arena, "argument to `first` not supported, got %S",
                     error_stringify_object_type(donkey_arg));
  }
  }

  return return_object;
}

Object _last(Arena *arena, Object *args) {
  if (len(args) > 1) {
    return new_error(arena, "wrong number of arguments. got=%d, want=1",
                     len(args));
  }

  Object donkey_arg = args[0];
  Object return_object = (Object){
      .eval_type = EVAL_OBJECT, //
  };

  switch (donkey_arg.type) {
  case ARRAY_OBJECT:
    //
    {
      ObjectArray donkey_array = donkey_arg.array;
      Object *array = donkey_array.value;
      if (len(array) == 0) {
        return DONKEY_PANIC_OBJECT;
      }
      I64 last_idx = len(array) - 1;
      I64 idx = (last_idx >= 0) ? last_idx : 0;
      return_object = array[idx];
      break;
    }
  default: {
    return new_error(arena, "argument to `last` not supported, got %S",
                     error_stringify_object_type(donkey_arg));
  }
  }

  return return_object;
}

Object _tail(Arena *arena, Object *args) {
  if (len(args) > 1) {
    return new_error(arena, "wrong number of arguments. got=%d, want=1",
                     len(args));
  }

  Object donkey_arg = args[0];
  Object return_object = (Object){
      .eval_type = EVAL_OBJECT, //
  };

  switch (donkey_arg.type) {
  case ARRAY_OBJECT:
    //
    {
      ObjectArray donkey_array = donkey_arg.array;
      Object *array = donkey_array.value;
      if (len(array) == 0) {
        return DONKEY_PANIC_OBJECT;
      }
      Object *array_tail = arena_array(arena, Object);
      Object item = {0};
      for (size_t i = 1; i < len(array); i++) {
        item = array[i];
        append(array_tail, item);
      }
      return_object.type = ARRAY_OBJECT;
      return_object.array.value = array_tail;
      break;
    }
  default: {
    return new_error(arena, "argument to `tail` not supported, got %S",
                     error_stringify_object_type(donkey_arg));
  }
  }

  return return_object;
}

Object _push(Arena *arena, Object *args) {
  if (len(args) < 2) {
    return new_error(arena, "wrong number of arguments. got=%d, want=2 or more",
                     len(args));
  }

  Object donkey_arg = args[0];
  Object return_object = (Object){
      .eval_type = EVAL_OBJECT, //
  };

  switch (donkey_arg.type) {
  case ARRAY_OBJECT:
    //
    {
      ObjectArray donkey_array = donkey_arg.array;
      Object *array = donkey_array.value;
      int n_append_items = len(args);
      int n_old_items = len(array);
      Object *new_array =
          arena_array_with_cap(arena, Object, n_old_items + n_append_items);

      /*byte *dest = (byte *)new_array;*/
      /*byte *source = (byte *)array;*/
      /**/
      /*memory_copy(dest, source, n_old_items * sizeof(Object));*/
      /*head(new_array)->length += n_old_items;*/
      /**/
      /*dest = (byte *)(new_array + n_old_items);*/
      /*source = (byte *)(args + 1);*/
      /**/
      /*memory_copy(dest, source, n_append_items * sizeof(Object));*/
      /*head(new_array)->length += n_append_items - 1;*/

      // this is the better code not error prone and that stuff
      for (int i = 0; i < len(array); i++) {
        Object appending = array[i];
        append(new_array, appending);
      }

      for (int i = 1; i < len(args); i++) {
        Object appending = args[i];
        append(new_array, appending);
      }

      return_object.type = ARRAY_OBJECT;
      return_object.array.value = new_array;
      break;
    }
  default: {
    return new_error(arena, "argument to `push` not supported, got %S",
                     error_stringify_object_type(donkey_arg));
  }
  }

  return return_object;
}

#define new_built_in(__fn)                                                     \
  (Object){.eval_type = EVAL_OBJECT,                                           \
           .type = BUILT_IN_OBJECT,                                            \
           .built_in.value = &__fn};

#define BUILT_IN_FNS                                                           \
  X(len)                                                                       \
  X(first)                                                                     \
  X(last)                                                                      \
  X(tail)                                                                      \
  X(push)

Object eval_evaluate_program(Arena *arena, Enviroment *env, Program program) {
  env_init(arena, &built_in_env);
#define X(fn) Object fn##_donkey = new_built_in(_##fn)
  BUILT_IN_FNS
#undef X
#define X(fn) env_insert_object(arena, &built_in_env, string(#fn), fn##_donkey);
  BUILT_IN_FNS
#undef X
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

#include "./object_hashmap.c"

String object_to_string(Arena *arena, Object object) {
  switch (object.type) {
  case INTEGER_OBJECT:
    //
    {
      return arena_string_fmt(arena, "%d", object.integer.value);
      break;
    }
  case STRING_OBJECT:
    //
    {
      return arena_string_fmt(arena, "'%S'", object.string.value);
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
  case ARRAY_OBJECT:
    //
    {
      ObjectArray array = object.array;
      Arena tmp_arena = {0};
      String members = arena_join_object_array(&tmp_arena, array.value);
      String result = arena_string_fmt(arena, "%S", members);
      arena_free(&tmp_arena);
      return result;
      break;
    }
  case HASH_MAP_OBJECT:
    //
    {
      ObjectHashMap hash_map = object.hash_map;
      HashTable *table = hash_map.value;
      KeyValueObject *items = table->items;
      size_t i, k;
      Arena tmp_arena = {0};
      String str_buffer = arena_new_empty_string_with_cap(arena, 64);
      String l_brace = string("{");
      String r_brace = string("}");
      String comma = string(", ");
      arena_string_concat(&tmp_arena, &str_buffer, l_brace);
      for (i = 0, k = 0; i < table->capacity; i++) {
        KeyValueObject key_value = items[i];
        if (!key_value.is_occupied) {
          continue;
        }
        if (k > 0) {
          arena_string_concat(&tmp_arena, &str_buffer, comma);
        }
        Object key = key_value.key;
        Object value = key_value.value;
        String key_str = object_to_string(arena, key);
        String value_str = object_to_string(arena, value);
        String kv_str =
            arena_string_fmt(&tmp_arena, "%S : %S", key_str, value_str);
        arena_string_concat(&tmp_arena, &str_buffer, kv_str);
        k++;
      }
      arena_string_concat(&tmp_arena, &str_buffer, r_brace);
      String result = arena_string_fmt(arena, "%S", str_buffer);
      arena_free(&tmp_arena);
      return result;
      break;
    }
  case ERROR_OBJECT:
    //
    {
      return arena_string_fmt(arena, "[ERROR]: %S", object.error.value);
      break;
    }
  case NIL_OBJECT:
    //
    {
      return arena_string_fmt(arena, "(null)");
      break;
    }
  default:
    //
    {
      return arena_string_fmt(arena, "(donk)%S", object.donkey);
      break;
    }
  }
}

void printo(Arena *arena, Object obj) {
  printfln("%S", object_to_string(arena, obj));
}
// built_in
