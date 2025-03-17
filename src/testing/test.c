#include "test.h"
#include "../object/object.h"
#include "test_evaluations.c"
#include "test_parser.c"
#include "test_tokens.c"
#include <stdio.h>

ResultEquals let_statement_equals(LetStatement *a, LetStatement *b) {
  ResultEquals result = {.are_equals = true,
                         .analysis =
                             arena_new_empty_string_with_cap(&arena, 512)};
  String error = {0};
  if (!token_equals(a->token, b->token)) {
    result.are_equals = false;
    error = arena_string_fmt(&arena,
                             "(a->token:%S) != (b->token:%S)\n", //
                             a->token.literal,                   //
                             b->token.literal                    //
    );
    analysis_append(error);
  }
  if (!string_equals(a->name.value, b->name.value)) {
    result.are_equals = false;
    error = arena_string_fmt(&arena,
                             "(%S) != (%S)\n", //
                             a->name.value,    //
                             b->name.value     //
    );
    analysis_append(error);
  }

  ResultEquals are_expression_equals =
      expression_equals(a->expression_value, b->expression_value);
  // TODO: change this for appending just the
  // analysis string from this ResultEquals
  if (!are_expression_equals.are_equals) {
    result.are_equals = false;
    String a_expression_string =
        stringify_expression(&arena, NULL, a->expression_value);
    String b_expression_string =
        stringify_expression(&arena, NULL, b->expression_value);
    error = arena_string_fmt(&arena,
                             "(%S) != (%S)\n",    //
                             a_expression_string, //
                             b_expression_string  //
    );
    analysis_append(error);
  }

  return result;
}

ResultEquals return_statement_equals(ReturnStatement *a, ReturnStatement *b) {
  ResultEquals result = {.are_equals = true,
                         .analysis =
                             arena_new_empty_string_with_cap(&arena, 512)};
  String error = {0};
  if (!token_equals(a->token, b->token)) {
    result.are_equals = false;
    error = arena_string_fmt(&arena,
                             LOG_ERROR "(a->token:%S) != (b->token:%S)\n", //
                             a->token.literal,                             //
                             b->token.literal                              //
    );
    analysis_append(error);
  }
  ResultEquals are_expression_equals =
      expression_equals(a->expression_value, b->expression_value);
  // TODO: change this for appending just the
  // analysis string from this ResultEquals
  if (!are_expression_equals.are_equals) {
    result.are_equals = false;
    String a_expression_string =
        stringify_expression(&arena, NULL, a->expression_value);
    String b_expression_string =
        stringify_expression(&arena, NULL, b->expression_value);
    error = arena_string_fmt(&arena,
                             "(%S) != (%S)\n",    //
                             a_expression_string, //
                             b_expression_string  //
    );
    analysis_append(error);
  }

  return result;
}

ResultEquals test_token_equals(Token a, Token b) {
  ResultEquals result = {.are_equals = true,
                         .analysis =
                             arena_new_empty_string_with_cap(&arena, 512)};
  String error = {0};

  if (!token_equals(a, b)) {
    result.are_equals = false;
    error = arena_string_fmt(&arena,
                             LOG_ERROR "%S != %S\n", //
                             a.literal,              //
                             b.literal               //
    );
    analysis_append(error);
  }

  return result;
}
// if i do refactor to this ones, i would rather have a global result
// equals where i just put all the stuff instead of 'allocating' in the
// arena each time i do an equals function
ResultEquals identifier_equals(Identifier a, Identifier b) {
  ResultEquals result = {.are_equals = true,
                         .analysis =
                             arena_new_empty_string_with_cap(&arena, 512)};
  ResultEquals tmp_result;
  String error = {0};
  // create context please
  tmp_result = test_token_equals(a.token, b.token);

  if (!tmp_result.are_equals) {
    result.are_equals = false;
    analysis_append(string(LOG_ERROR "Tokens are not the same\n"));
    analysis_append(tmp_result.analysis);
  }

  if (!string_equals(a.value, b.value)) {
    result.are_equals = false;
    analysis_append(string(LOG_ERROR "Identifier names are not the same\n"));
    error = arena_string_fmt(&arena,
                             LOG_ERROR "%S != %S\n", //
                             a.value,                //
                             b.value                 //
    );
    analysis_append(error);
  }

  return result;
}

ResultEquals integer_literal_equals(IntLiteral a, IntLiteral b) {

  ResultEquals result = {.are_equals = true,
                         .analysis =
                             arena_new_empty_string_with_cap(&arena, 512)};
  ResultEquals tmp_result;
  String error = {0};
  tmp_result = test_token_equals(a.token, b.token);

  if (!tmp_result.are_equals) {
    result.are_equals = false;
    analysis_append(string(LOG_ERROR "Tokens are not the same\n"));
    analysis_append(tmp_result.analysis);
  }

  if (a.value != b.value) {
    result.are_equals = false;
    analysis_append(
        string(LOG_ERROR "Integer literal values are not the same\n"));
    error = arena_string_fmt(&arena,
                             LOG_ERROR "%d != %d\n", //
                             a.value,                //
                             b.value                 //
    );
    analysis_append(error);
  }

  return result;
}
ResultEquals boolean_equals(Boolean a, Boolean b) {

  ResultEquals result = {.are_equals = true,
                         .analysis =
                             arena_new_empty_string_with_cap(&arena, 512)};
  ResultEquals tmp_result;
  String error = {0};
  tmp_result = test_token_equals(a.token, b.token);

  if (!tmp_result.are_equals) {
    analysis_append(string(LOG_ERROR "Tokens are not the same\n"));
    result.are_equals = false;
    analysis_append(tmp_result.analysis);
  }

  if (a.value != b.value) {
    analysis_append(string(LOG_ERROR "Boolean values are not the same\n"));
    result.are_equals = false;
    error = arena_string_fmt(&arena,
                             LOG_ERROR "%b != %b\n", //
                             a.value,                //
                             b.value                 //
    );
    analysis_append(error);
  }

  return result;
}
ResultEquals prefix_equals(PrefixExpression a, PrefixExpression b) {
  ResultEquals result = {.are_equals = true,
                         .analysis =
                             arena_new_empty_string_with_cap(&arena, 512)};
  ResultEquals tmp_result;
  String error = {0};
  tmp_result = test_token_equals(a.token, b.token);

  if (!tmp_result.are_equals) {
    analysis_append(string(LOG_ERROR "Prefix: Tokens are not the same\n"));
    result.are_equals = false;
    analysis_append(tmp_result.analysis);
  }

  if (!string_equals(a.operator, b.operator)) {
    analysis_append(string(LOG_ERROR "Pefix: Operators are not the same\n"));
    result.are_equals = false;
    error = arena_string_fmt(&arena,
                             LOG_ERROR "%S != %S\n", //
                             a.operator,             //
                             b.operator              //
    );
    analysis_append(error);
  }

  // memory being dupped in a silly way!
  ResultEquals are_right_exps_equal = expression_equals(a.right, b.right);

  if (!are_right_exps_equal.are_equals) {
    analysis_append(
        string(LOG_ERROR "Pefix: Right operations are not the same\n"));
    result.are_equals = false;
    analysis_append(are_right_exps_equal.analysis);
  }

  return result;
}
ResultEquals infix_equals(InfixExpression a, InfixExpression b) {
  ResultEquals result = {.are_equals = true,
                         .analysis =
                             arena_new_empty_string_with_cap(&arena, 512)};
  ResultEquals tmp_result;
  String error = {0};
  tmp_result = test_token_equals(a.token, b.token);

  if (!tmp_result.are_equals) {
    analysis_append(string(LOG_ERROR "Infix: tokens are not the same\n"));
    result.are_equals = false;
    analysis_append(tmp_result.analysis);
  }

  if (!string_equals(a.operator, b.operator)) {
    analysis_append(string(LOG_ERROR "Infix: operators are not the same\n"));
    result.are_equals = false;
    error = arena_string_fmt(&arena,
                             LOG_ERROR "%S != %S\n", //
                             a.operator,             //
                             b.operator              //
    );
    analysis_append(error);
  }

  // memory being dupped in a silly way!
  ResultEquals are_right_exps_equal = expression_equals(a.right, b.right);

  if (!are_right_exps_equal.are_equals) {
    analysis_append(
        string(LOG_ERROR "Infix: right expressions are not the same\n"));
    result.are_equals = false;
    analysis_append(are_right_exps_equal.analysis);
  }

  ResultEquals are_left_exps_equal = expression_equals(a.left, b.left);

  if (!are_left_exps_equal.are_equals) {
    analysis_append(
        string(LOG_ERROR "Infix: left expressions are not the same\n"));
    result.are_equals = false;
    analysis_append(are_left_exps_equal.analysis);
  }

  return result;
}

ResultEquals if_expression_equals(IfExpression a, IfExpression b) {

  ResultEquals result = {.are_equals = true,
                         .analysis =
                             arena_new_empty_string_with_cap(&arena, 512)};
  ResultEquals tmp_result;
  tmp_result = test_token_equals(a.token, b.token);

  if (!tmp_result.are_equals) {
    analysis_append(
        string(LOG_ERROR "if_expression_equals: tokens are not the same\n"));
    result.are_equals = false;
    analysis_append(tmp_result.analysis);
  }
  // memory being dupped in a silly way!
  ResultEquals are_condition_exps_equal =
      expression_equals(a.condition, b.condition);

  if (!are_condition_exps_equal.are_equals) {
    analysis_append(string(
        LOG_ERROR "if_expression_equals: conditions are not the same\n"));
    result.are_equals = false;
    analysis_append(are_condition_exps_equal.analysis);
  }

  // TODO: wrap this into a macro

  ResultEquals are_consequences_equal =
      block_statement_equals(a.consequence, b.consequence);

  if (!are_consequences_equal.are_equals) {
    analysis_append(string(
        LOG_ERROR "if_expression_equals: consequences are not the same\n"));
    result.are_equals = false;
    analysis_append(are_consequences_equal.analysis);
  }

  ResultEquals are_alternatives_equal =
      block_statement_equals(a.alternative, b.alternative);

  if (!are_alternatives_equal.are_equals) {
    analysis_append(string(
        LOG_ERROR "if_expression_equals: alternatives are not the same\n"));
    result.are_equals = false;
    analysis_append(are_alternatives_equal.analysis);
  }

  return result;
}

// TODO: maybe put in a future a context parameter where i pass like a string to
// tell when is this function being used
ResultEquals block_statement_equals(BlockStatement a, BlockStatement b) {
  ResultEquals result = {.are_equals = true,
                         .analysis =
                             arena_new_empty_string_with_cap(&arena, 512)};
  ResultEquals tmp_result;
  String error = {0};

  if (len(a.statements) != len(b.statements)) {
    analysis_append(string(LOG_ERROR
                           "block_statement_equals: blocks statements lengths "
                           "are not the same\n"));
    result.are_equals = false;
    error = arena_string_fmt(&arena, LOG_ERROR
                             "They dont have the same amount of statements\n");
    analysis_append(error);
    return result;
  }

  for (I64 i = 0; i < len(a.statements); i++) {
    Node *statement_a = &a.statements[i];
    Node *statement_b = &b.statements[i];
    tmp_result = node_equals(statement_a, statement_b);
    if (!tmp_result.are_equals) {
      analysis_append(string(
          LOG_ERROR "block_statement_equals: statements are not the same\n"));
      result.are_equals = false;
      analysis_append(tmp_result.analysis);
    }
  }
  /*arena_free(tmp_result); :( someday thouh!
   * */
  return result;
}

ResultEquals function_literal_equals(FunctionLiteral a, FunctionLiteral b) {
  ResultEquals result = {.are_equals = true,
                         .analysis =
                             arena_new_empty_string_with_cap(&arena, 512)};
  ResultEquals tmp_result;
  String error = {0};
  tmp_result = test_token_equals(a.token, b.token);

  if (!tmp_result.are_equals) {
    result.are_equals = false;
    analysis_append(tmp_result.analysis);
  }

  tmp_result = identifier_equals(a.name, b.name);

  if (!tmp_result.are_equals) {
    result.are_equals = false;
    analysis_append(tmp_result.analysis);
  }

  I64 a_len = len(a.parameters);
  I64 b_len = len(b.parameters);
  if (a_len != b_len) {
    result.are_equals = false;
    error = arena_string_fmt(
        &arena,
        LOG_ERROR "parameters length does not match (a: %d) != (b: %d)\n", //
        a_len,                                                             //
        b_len);
    analysis_append(error);
  } else {
    Identifier a_identifier;
    Identifier b_identifier;
    for (I64 i = 0; i < a_len; i++) {
      a_identifier = a.parameters[i];
      b_identifier = b.parameters[i];
      tmp_result = identifier_equals(a_identifier, b_identifier);
      if (!tmp_result.are_equals) {
        result.are_equals = false;
        analysis_append(tmp_result.analysis);
      }
    }
  }

  ResultEquals are_bodies_equal = block_statement_equals(a.body, b.body);

  if (!are_bodies_equal.are_equals) {
    result.are_equals = false;
    analysis_append(are_bodies_equal.analysis);
  }

  return result;
}

ResultEquals function_call_equals(FunctionCallExpression a,
                                  FunctionCallExpression b) {

  ResultEquals result = {.are_equals = true,
                         .analysis =
                             arena_new_empty_string_with_cap(&arena, 512)};
  ResultEquals tmp_result;
  String error = {0};
  tmp_result = test_token_equals(a.token, b.token);

  if (!tmp_result.are_equals) {
    result.are_equals = false;
    analysis_append(tmp_result.analysis);
  }

  I64 a_len = len(a.arguments);
  I64 b_len = len(b.arguments);

  if (a_len != b_len) {
    result.are_equals = false;
    error = arena_string_fmt(
        &arena,
        LOG_ERROR "arguments length does not match (a: %d) != (b: %d)\n", //
        a_len,                                                            //
        b_len);
    analysis_append(error);
  } else {
    Expression *a_expression;
    Expression *b_expression;
    for (I64 i = 0; i < a_len; i++) {
      a_expression = a.arguments[i];
      b_expression = b.arguments[i];
      tmp_result = expression_equals(a_expression, b_expression);
      if (!tmp_result.are_equals) {
        result.are_equals = false;
        analysis_append(tmp_result.analysis);
      }
    }
  }

  ResultEquals are_function_identifiers =
      expression_equals(a.function, b.function);

  if (!are_function_identifiers.are_equals) {
    result.are_equals = false;
    analysis_append(are_function_identifiers.analysis);
  }

  return result;
}

ResultEquals expression_equals(Expression *a, Expression *b) {
  ResultEquals result = {.are_equals = true,
                         .analysis =
                             arena_new_empty_string_with_cap(&arena, 512)};
  // this would be an amazing use of a temp arena btw maybe in a future refactor
  String error = {0};
  if (a->type != b->type) {
    result.are_equals = false;
    error = arena_string_fmt(&arena,
                             LOG_ERROR "(a->type:%s) != (b->type:%s)\n", //
                             NODE_TYPES[a->type],                        //
                             NODE_TYPES[b->type]                         //
    );
    analysis_append(error);
    return result;
  }

  switch (a->type) {
  case IDENTIFIER_EXP:
    //
    {
      result = identifier_equals(a->identifier, b->identifier);
      break;
    }
  case INTEGER_LIT_EXP:
    //
    {
      result = integer_literal_equals(a->integer_literal, b->integer_literal);
      break;
    }
  case BOOLEAN_EXP:
    //
    {
      result = boolean_equals(a->boolean, b->boolean);
      break;
    }
  case PREFIX_EXP:
    //
    {
      result = prefix_equals(a->prefix, b->prefix);
      break;
    }
  case INFIX_EXP:
    //
    {
      result = infix_equals(a->infix, b->infix);
      break;
    }
  case IF_EXP:
    //
    {
      result = if_expression_equals(a->if_expression, b->if_expression);
      break;
    }
  case FUNCTION_LITERAL_EXP:
    //
    {
      result =
          function_literal_equals(a->function_literal, b->function_literal);
      break;
    }
  case FUNCTION_CALL_EXP:
    //
    {
      result = function_call_equals(a->function_call, b->function_call);
      break;
    }
  default:
    //
    {
      result.are_equals = false;
      error =
          arena_string_fmt(&arena,
                           LOG_ERROR "dunno what happened but it happened\n" //
          );
      analysis_append(error);
      break;
    }
  }

  String a_expression_string = stringify_expression(&arena, NULL, a);
  String b_expression_string = stringify_expression(&arena, NULL, b);
  String final_msg;
  char *log = LOG_SUCCESS;
  char *symbol = "==";
  if (!result.are_equals) {
    result.are_equals = false;
    symbol = "!=";
    log = LOG_ERROR;
  }
  final_msg = arena_string_fmt(&arena,
                               "\n%s%S %s %S\n",    //
                               log,                 //
                               a_expression_string, //
                               symbol,              //
                               b_expression_string);
  analysis_append(final_msg);
  return result;
}

ResultEquals node_equals(Node *a, Node *b) {
  ResultEquals result = {.are_equals = true,
                         .analysis =
                             arena_new_empty_string_with_cap(&arena, 512)};

  // this would be an amazing use of a temp arena btw maybe in a future refactor
  String error = {0};
  if (a->type != b->type) {
    result.are_equals = false;
    error = arena_string_fmt(&arena,
                             LOG_ERROR "(a->type:%s) != (b->type:%s)\n", //
                             NODE_TYPES[a->type],                        //
                             NODE_TYPES[b->type]                         //
    );
    analysis_append(error);
    return result;
  }
  switch (a->type) {
  case LET_STATEMENT:
    return let_statement_equals(&a->let_statement, &b->let_statement);
  case RETURN_STATEMENT:
    return return_statement_equals(&a->return_statement, &b->return_statement);
  case EXPRESSION_STATEMENT:
    return expression_equals(a->expression_statement.expression_value,
                             b->expression_statement.expression_value);
  default:
    result.are_equals = false;
    analysis_append(string("Statement type is nil so check that out"));
    break;
  }
  return result;
}

Node *new_node(char *input) {
  Lexer lexer = lexer_new_lexer(string(input));
  Parser parser = ast_new_parser(&arena, &lexer);
  Node *node = ast_parse_statement(&arena, &parser);
  /*print_parser_errors(parser);*/
  return node;
}

void test_print_expression(Expression *expression) {
  String expression_string = stringify_expression(&arena, NULL, expression);
  printf("%s\n", expression_string.str);
}

void test() {
  /*Node *a = new_node("if (x > y){return 1;}else{return 2;}");*/
  /*Node *a = new_node("fn foo(x,y){let uwu = 3;}");*/
  Node *a = new_node("fn foo(x,y){let uwu = 3;}");
  Node *b = new_node("fn caca(x,y){return -2;}");
  ResultEquals res = node_equals(a, b);
  if (res.are_equals) {
    printf(LOG_SUCCESS "everything stays\n");
    printf("%s\n", res.analysis.str);
  } else {
    printf(LOG_ERROR "oopssie\n" end_color);
    printf("%s\n", res.analysis.str);
  }
}

void print_object_types() {
#define X(name) printf("%s\n", _ObjectToString[name]);
  OBJECT_TYPES
#undef X
}
// TODO: put number of tests failed and passed just a counter girllll and if u
// wanna get nitty and gritty use a hashtable {or just a struct array that
// contains the number of the test, the name of the function of the test or just
// the input test string yeah that sounds better andddd if passed or not}
int main() {
  test_built_in_functions();
  test_error_handling();
  test_parser();
  test_string_evaluation();
  test_token();
  test_generic();
  test_function_application();
  /*donkey_repl(&arena);*/
  /*test_repl();*/
  test_let_statements_integers();
  printf("\n");
  test_integer_evaluations();
  printf("\n");
  printf("\n");
  test_if_expressions_evaluations();
  printf("\n");
  test_bool_evaluations();
  printf("\n");
  test_return_expressions_evaluations();
  printf("\n");
  return 0;
}
