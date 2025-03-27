#ifndef _DONKEY_HASH_MAP_H
#define _DONKEY_HASH_MAP_H
#include "../arena_hash_table.c"
#include "./ast.c"
#include <stdbool.h>

// one nut idea would be to stringify all the expressions and just compare the
// strings asdkfasdf which is a 'good' idea if i just wanna end with this but i
// spent so much time writing the parser so lets feel it in our hands if u could
// see i am the one who understands youuu uuu belong with meeeeee eyou belong
// with me eeeeeeeeee walk in the streeets with you and your worn out jeans
// laguing in a park bench hey isnt this easy light up this whole town in a
// while since she boroguh u donwwwwwwwww girtl like that i wear cheer captain
// dreaming about the day olloo kin is askdjfkasdjfsd
bool are_array_equals(Array a, Array b);
bool are_index_equals(Index a, Index b);
bool are_prefixs_equals(PrefixExpression a, PrefixExpression b);
bool are_infixs_equals(InfixExpression a, InfixExpression b);
bool are_if_expressions_equals(IfExpression a, IfExpression b);
bool are_function_literals_equals(FunctionLiteral a, FunctionLiteral b);
bool are_function_calls_equals(FunctionCallExpression a,
                               FunctionCallExpression b);
bool are_hash_maps_equals_(HashLiteral_ a, HashLiteral_ b);
bool are_key_hashes_equals_equals(KeyHash a, KeyHash b);
bool are_booleans_equals(Boolean a, Boolean b);
bool are_identifier_equals(Identifier a, Identifier b);
bool are_integer_equals(IntLiteral a, IntLiteral b);
bool are_string_literals_equals(StringLiteral a, StringLiteral b);
bool are_expressions_equals(void *a, void *b);
bool are_nodes_equals(Node *a, Node *b);
Expression *donkey_hash_map_get_item(HashTable *table, Expression *key);
void donkey_hash_map_insert(Arena *arena, HashTable *table, Expression *key,
                            Expression *value);

bool are_array_equals(Array a, Array b) {
  bool are_token_equals = token_equals(a.token, b.token);
  bool are_members_equals = true;

  if (len(a.value) != len(b.value)) {
    are_members_equals = false;
  } else {
    for (size_t i = 0; i < len(a.value); i++) {
      Expression *a_exp = a.value[i];
      Expression *b_exp = b.value[i];
      if (!are_expressions_equals(a_exp, b_exp)) {
        are_members_equals = false;
        break;
      };
    }
  }

  return are_members_equals && are_token_equals;
}

bool are_index_equals(Index a, Index b) {
  bool are_token_equals = token_equals(a.token, b.token);
  bool are_indexes_equals = are_expressions_equals(a.index, b.index);
  bool are_data_equals = are_expressions_equals(a.data, b.data);

  return are_token_equals && are_indexes_equals && are_data_equals;
}

bool are_prefixs_equals(PrefixExpression a, PrefixExpression b) {
  bool are_token_equals = token_equals(a.token, b.token);
  bool are_operator_equals = string_equals(a.operator, b.operator);
  bool are_rights_equals = are_expressions_equals(a.right, b.right);
  return are_token_equals && are_operator_equals && are_rights_equals;
}

bool are_infixs_equals(InfixExpression a, InfixExpression b) {
  bool are_token_equals = token_equals(a.token, b.token);
  bool are_rights_equals = are_expressions_equals(a.right, b.right);
  bool are_lefts_equals = are_expressions_equals(a.left, b.left);
  bool are_operator_equals = string_equals(a.operator, b.operator);
  return are_token_equals && are_lefts_equals && are_operator_equals &&
         are_rights_equals;
}

bool are_block_statements_equals(BlockStatement a, BlockStatement b) {
  if (len(a.statements) != len(b.statements)) {
    return false;
  }

  bool are_statements_equals = true;
  for (size_t i = 0; i < len(a.statements); i++) {
    Node *a_node = &a.statements[i];
    Node *b_node = &b.statements[i];
    if (!are_nodes_equals(a_node, b_node)) {
      are_statements_equals = false;
      break;
    }
  }

  return are_statements_equals;
}

bool are_if_expressions_equals(IfExpression a, IfExpression b) {
  bool are_token_equals = token_equals(a.token, b.token);
  bool are_conditions_equals = are_expressions_equals(a.condition, b.condition);
  bool are_consequences_equals =
      are_block_statements_equals(a.consequence, b.consequence);
  bool are_alternatives_equals =
      are_block_statements_equals(a.alternative, b.alternative);
  return are_token_equals && are_conditions_equals && are_consequences_equals &&
         are_alternatives_equals;
}

bool are_function_literals_equals(FunctionLiteral a, FunctionLiteral b) {
  bool are_token_equals = token_equals(a.token, b.token);
  bool are_names_equals = are_identifier_equals(a.name, b.name);
  bool are_body_equals = are_block_statements_equals(a.body, b.body);
  bool are_parameters_equals = true;
  if (len(a.parameters) != len(b.parameters)) {
    are_parameters_equals = false;
  } else {
    for (size_t i = 0; i < len(a.parameters); i++) {
      Identifier a_param = a.parameters[i];
      Identifier b_param = b.parameters[i];
      if (!are_identifier_equals(a_param, b_param)) {
        are_parameters_equals = false;
        break;
      }
    }
  }
  return are_token_equals && are_names_equals && are_body_equals &&
         are_parameters_equals;
}

bool are_function_calls_equals(FunctionCallExpression a,
                               FunctionCallExpression b) {
  bool are_token_equals = token_equals(a.token, b.token);
  bool are_functions_equals = are_expressions_equals(a.function, b.function);
  bool are_arguments_equals = true;
  if (len(a.arguments) != len(b.arguments)) {
    are_arguments_equals = false;
  } else {
    for (size_t i = 0; i < len(a.arguments); i++) {
      Expression *a_arg = a.arguments[i];
      Expression *b_arg = b.arguments[i];
      if (!are_expressions_equals(a_arg, b_arg)) {
        are_arguments_equals = false;
        break;
      }
    }
  }
  return are_token_equals && are_functions_equals && are_arguments_equals;
}

typedef struct {
  Expression *key;
  bool is_occupied;
  Expression *value;
} KeyValueExpression;

bool are_key_hashes_equals_equals(KeyHash a, KeyHash b) {
  bool are_token_equals = token_equals(a.token, b.token);
  if (!a.hash_map || !b.hash_map) {
    return false;
  }
  bool are_hash_literals_equals =
      are_hash_maps_equals_(*a.hash_map, *b.hash_map);
  bool are_index_equals = are_expressions_equals(a.index, b.index);
  return are_token_equals && are_hash_literals_equals && are_index_equals;
}

bool are_booleans_equals(Boolean a, Boolean b) {
  bool are_token_equals = token_equals(a.token, b.token);
  return are_token_equals;
}

bool are_identifier_equals(Identifier a, Identifier b) {
  bool are_name_equals = string_equals(a.value, b.value);
  bool are_token_equals = token_equals(a.token, b.token);
  return are_name_equals && are_token_equals;
}

bool are_integer_equals(IntLiteral a, IntLiteral b) {
  bool are_integer_value_equals = a.value == b.value;
  bool are_token_equals = token_equals(a.token, b.token);
  return are_integer_value_equals && are_token_equals;
}

bool are_string_literals_equals(StringLiteral a, StringLiteral b) {
  bool are_string_value_equals = string_equals(a.value, b.value);
  bool are_token_equals = token_equals(a.token, b.token);
  return are_string_value_equals && are_token_equals;
}

bool are_let_statement_equals(LetStatement a, LetStatement b) {
  bool are_token_equals = token_equals(a.token, b.token);
  bool are_names_equals = are_identifier_equals(a.name, b.name);
  bool are_expression_values_equals =
      are_expressions_equals(a.expression_value, b.expression_value);

  return are_token_equals && are_names_equals && are_expression_values_equals;
}

bool are_return_statement_equals(ReturnStatement a, ReturnStatement b) {
  bool are_token_equals = token_equals(a.token, b.token);
  bool are_expression_values_equals =
      are_expressions_equals(a.expression_value, b.expression_value);

  return are_token_equals && are_expression_values_equals;
}

bool are_expression_statement_equals(ExpressionStatement a,
                                     ExpressionStatement b) {
  bool are_token_equals = token_equals(a.token, b.token);
  bool are_expression_values_equals =
      are_expressions_equals(a.expression_value, b.expression_value);

  return are_token_equals && are_expression_values_equals;
}


bool are_nodes_equals(Node *a, Node *b) {
  bool are_token_equals = token_equals(a->token, b->token);
  if (a->type != b->type || !are_token_equals) {
    return false;
  }
  switch (a->type) {
  case LET_STATEMENT:
    //
    {
      LetStatement let_statement_a = a->let_statement;
      LetStatement let_statement_b = b->let_statement;
      return are_let_statement_equals(let_statement_a, let_statement_b);
    }
  case RETURN_STATEMENT:
    //
    {
      ReturnStatement return_statement_a = a->return_statement;
      ReturnStatement return_statement_b = b->return_statement;
      return are_return_statement_equals(return_statement_a,
                                         return_statement_b);
      break;
    }
  case EXPRESSION_STATEMENT:
    //
    {
      ExpressionStatement expression_statement_a = a->expression_statement;
      ExpressionStatement expression_statement_b = b->expression_statement;
      return are_expression_statement_equals(expression_statement_a,
                                             expression_statement_b);
      break;
    }
  default:
    //
    {
      printf("bro");
      return false;
      break;
    }
  }

}

bool are_hash_maps_equals_(HashLiteral_ a_hash_map, HashLiteral_ b_hash_map) {
  (void) a_hash_map;
  (void) b_hash_map;
  return true;
}

bool are_expressions_equals(void *a_ptr, void *b_ptr) {
  Expression *a = (Expression *)a_ptr;
  Expression *b = (Expression *)b_ptr;
  if (!a || !b) {
    return false;
  }
  if (a->type != b->type) {
    return false;
  }
  switch (a->type) {
  case IDENTIFIER_EXP:
    //
    {
      Identifier a_identifier = a->identifier;
      Identifier b_identifier = b->identifier;
      return are_identifier_equals(a_identifier, b_identifier);
    }
  case INTEGER_LIT_EXP:
    //
    {
      IntLiteral a_integer = a->integer_literal;
      IntLiteral b_integer = b->integer_literal;
      return are_integer_equals(a_integer, b_integer);
      break;
    }
  case STRING_LIT_EXP:
    //
    {
      StringLiteral a_string = a->string_literal;
      StringLiteral b_string = b->string_literal;
      return are_string_literals_equals(a_string, b_string);
      break;
    }
  case ARRAY_EXP:
    //
    {
      Array a_array = a->array;
      Array b_array = b->array;
      return are_array_equals(a_array, b_array);
      break;
    }
  case INDEX_EXP:
    //
    {

      Index a_index = a->index;
      Index b_index = b->index;
      return are_index_equals(a_index, b_index);
      break;
    }
  case BOOLEAN_EXP:
    //
    {
      Boolean a_boolean = a->boolean;
      Boolean b_boolean = b->boolean;
      return are_booleans_equals(a_boolean, b_boolean);
      break;
    }
  case PREFIX_EXP:
    //
    {
      PrefixExpression a_prefix = a->prefix;
      PrefixExpression b_prefix = b->prefix;
      return are_prefixs_equals(a_prefix, b_prefix);
      break;
    }
  case INFIX_EXP:
    //
    {
      InfixExpression a_infix = a->infix;
      InfixExpression b_infix = b->infix;
      return are_infixs_equals(a_infix, b_infix);
      break;
    }
  case IF_EXP:
    //
    {
      IfExpression a_if_expression = a->if_expression;
      IfExpression b_if_expression = b->if_expression;
      return are_if_expressions_equals(a_if_expression, b_if_expression);
      break;
    }
  case FUNCTION_LITERAL_EXP:
    //
    {
      FunctionLiteral a_function_literal = a->function_literal;
      FunctionLiteral b_function_literal = b->function_literal;
      return are_function_literals_equals(a_function_literal,
                                          b_function_literal);
      break;
    }
  case FUNCTION_CALL_EXP:
    //
    {
      FunctionCallExpression a_function_call = a->function_call;
      FunctionCallExpression b_function_call = b->function_call;
      return are_function_calls_equals(a_function_call, b_function_call);
      break;
    }
  case HASH_MAP_EXP:
    //
    {
      HashLiteral_ a_hash_map = a->hash_literal_;
      HashLiteral_ b_hash_map = b->hash_literal_;
      return are_hash_maps_equals_(a_hash_map, b_hash_map);
      break;
    }
  case KEY_HASH_MAP_EXP:
    //
    {
      KeyHash a_key_hash = a->key_hash;
      KeyHash b_key_hash = b->key_hash;
      return are_key_hashes_equals_equals(a_key_hash, b_key_hash);
      break;
    }
  default:
    //
    {
      printf("why did we got here");
      return false;
    }
  }
}

// TODO: hashmap cool implementation use void pointers and because we have a
// equals function we dont actually need them to be a fixed type

#include "../arena_hash_table.c"

U64 donkey_get_hash(void *key_ptr) {
  Expression *expression = (Expression *)key_ptr;
  Arena tmp_arena = {0};
  String key = stringify_expression(&tmp_arena, NULL, expression);
  U64 d = 0x01000193;
  for (U64 i = 0; i < key.len; i++) {
    d = ((d * 0x01000193) ^ key.str[i]) & 0xffffffff;
  }
  arena_free(&tmp_arena);
  return d;
}

void donkey_hash_map_insert(Arena *arena, HashTable *table, Expression *key,
                            Expression *value) {

  if (!table->items) {
    hash_table_alloc_custom(arena, table, KeyValueExpression,
                            are_expressions_equals, donkey_get_hash);
  }

  if (table->len >= table->capacity) {
    table->items =
        realloc(table->items, table->capacity * 2 * table->item_size);
    table->capacity = table->capacity * 2 * table->item_size;
  }

  // xd
  U64 hash = table->get_hash(key) % (table->capacity);
  KeyValueExpression *items = (table)->items;
  KeyValueExpression *curr_item = &items[hash];

  while (curr_item->is_occupied &&
         !(table)->are_keys_equals(curr_item->key, key) &&
         hash < (table)->capacity) {
    hash++;
    curr_item = &items[hash];
  }

  if (curr_item->is_occupied &&
      !((table)->are_keys_equals(curr_item->key, key))) {
    printf("OUT OF MEMORY IN DONKEYLANG\n");
    return;
  }

  // if i dont wanna support redeclaration with lets
  // just putting the assigmnet here would do the
  // job
  if (!curr_item->is_occupied) {
    table->len++;
  }

  curr_item->key = key;
  curr_item->is_occupied = true;
  curr_item->value = value;
}

Expression *donkey_hash_map_get_item(HashTable *table, Expression *key) {
  if (!table || !(table)->items) {
    return NULL;
  }
  U64 hash = (table)->get_hash(key) % (table)->capacity;
  KeyValueExpression *items = (table)->items;
  KeyValueExpression *curr_item = &items[hash];

  while (curr_item->is_occupied &&
         !(table)->are_keys_equals(curr_item->key, key) &&
         hash < (table)->capacity) {
    hash++;
    curr_item = &items[hash];
  }

  bool are_keys_equals = (table)->are_keys_equals(curr_item->key, key);

  if (!are_keys_equals) {
    return NULL;
  }

  return curr_item->value;
}

#endif /* ifndef _DONKEY_HASH_MAP_H */
