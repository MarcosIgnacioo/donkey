#include "test.h"
#include <stdio.h>

ResultEquals let_statement_equals(LetStatement *a, LetStatement *b) {
  ResultEquals result = {.are_equals = true,
                         .analysis =
                             arena_new_empty_string_with_cap(&arena, 512)};
  String error = {0};
  if (token_equals(a->token, b->token)) {
    error = arena_string_fmt(&arena,
                             LOG_ERROR "(a->token:%S) != (b->token:%S)", //
                             a->token.literal,                           //
                             b->token.literal                            //
    );
    analysis_append(error);
  }
  if (string_equals(a->name.value, b->name.value)) {
    error = arena_string_fmt(&arena,
                             LOG_ERROR "Variables names are not equal\n"
                                       "(%S) != (%S)", //
                             a->name.value,            //
                             b->name.value             //
    );
    analysis_append(error);
  }

  ResultEquals are_expression_equals =
      expression_equals(a->expression_value, b->expression_value);
  // TODO: change this for appending just the
  // analysis string from this ResultEquals
  if (!are_expression_equals.are_equals) {
    String a_expression_string =
        stringify_expression(&arena, NULL, a->expression_value);
    String b_expression_string =
        stringify_expression(&arena, NULL, b->expression_value);
    error =
        arena_string_fmt(&arena,
                         LOG_ERROR "Let Statement expressions are not equal\n"
                                   "(%S) != (%S)", //
                         a_expression_string,      //
                         b_expression_string       //
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
  if (token_equals(a->token, b->token)) {
    error = arena_string_fmt(&arena,
                             LOG_ERROR "(a->token:%S) != (b->token:%S)", //
                             a->token.literal,                           //
                             b->token.literal                            //
    );
    analysis_append(error);
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
  }
  String a_expression_string = stringify_expression(&arena, NULL, a);
  String b_expression_string = stringify_expression(&arena, NULL, b);
  String final_msg;
  char *log = LOG_SUCCESS;
  char *symbol = "==";
  if (!result.are_equals) {
    symbol = "!=";
    log = LOG_ERROR;
  }
  final_msg = arena_string_fmt(&arena,
                               "%s%S %s %S",        //
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
                             LOG_ERROR "(a->type:%s) != (b->type:%s)", //
                             NODE_TYPES[a->type],                      //
                             NODE_TYPES[b->type]                       //
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
  return node;
}

void test_print_expression(Expression *expression) {
  String expression_string = stringify_expression(&arena, NULL, expression);
  printf("%s\n", expression_string.str);
}

void test() {
  /*Node *a = new_node("if (x > y){return 1;}else{return 2;}");*/
  /*Node *a = new_node("fn foo(x,y){let uwu = 3;}");*/
  Node *a = new_node("foo(x,y + 2 * (3 - 3))");
  Node *b = new_node("foo");
  ResultEquals res = node_equals(a, b);
  if (res.are_equals) {
    printf(LOG_SUCCESS "everything stays\n");
    printf(LOG_SUCCESS "%s\n", res.analysis.str);
  } else {
    printf("oopssie\n");
    printf("%s\n", res.analysis.str);
  }
}

int main() {
  test();
  return 0;
}
