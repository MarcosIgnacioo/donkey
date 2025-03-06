#ifndef _TEST_H
#define _TEST_H
#include "../array.c"
#include "../lexer.c"
#include "../object/object.h"
#include "../parser/ast.c"
#include "../repl.c"
#include "../token.c"
#include <assert.h>
#include <stdio.h>
#define color(C) "\033[0;3" #C "m"
#define stringify(VAR) #VAR
#define end_color "\033[0m"
#define LOG_ERROR color(3) "[TEST ERROR] : " end_color
#define LOG_SUCCESS color(2) "[SUCCESS] : " end_color
#define LOG_INFO color(4) "[TEST INFO] : " end_color
#define print_error(...) printf(LOG_ERROR __VA_ARGS__)

Arena arena = {0};

bool _expression_equalsowo(Expression *exp_a, Expression *exp_b) {
  bool are_equal = true;
  assert(exp_a->type == INFIX_EXP);
  assert(exp_b->type == INFIX_EXP);
  InfixExpression infix_a = exp_a->infix;
  InfixExpression infix_b = exp_b->infix;

  if (!token_equals(infix_a.token, infix_b.token)) {
    are_equal = false;
    print_error("Tokens are not equal");
  }

  if (!_expression_equalsowo(infix_a.left, infix_b.left)) {
    are_equal = false;
    print_error("Lefts are not equal");
  }

  if (!string_equals(infix_a.operator, infix_b.operator)) {
    are_equal = false;
    print_error("Operators are not equal");
  }

  return are_equal;
}

typedef enum {
  NIL,
  NOT_SAME_TYPE,
  NOT_SAME_TOKEN,
  // infix errors
  NOT_SAME_OPERATOR,
  NOT_SAME_LEFT,
  NOT_SAME_RIGHT,
  NOT_SAME_FUNCTION,
  NOT_SAME_ARGUMENTS,
  NOT_SAME_PARAMETERS,
  NOT_SAME_BODY,
  NOT_SAME_FUNCTION_NAME,
} ExpressionEqualsError;

typedef struct {
  bool are_equals;
  String analysis;
} ResultEquals;

const char *NODE_TYPES[] = {
    "NIL_STATEMENT",
    "LET_STATEMENT",
    "RETURN_STATEMENT",
    "EXPRESSION_STATEMENT",
    "IDENTIFIER_EXP",
    "INTEGER_LIT_EXP",
    "BOOLEAN_EXP",
    "PREFIX_EXP",
    "INFIX_EXP",
    "IF_EXP",
    "FUNCTION_LITERAL_EXP",
    "FUNCTION_CALL_EXP",
};

#define analysis_append(ANALYSIS)                                              \
  arena_string_concat(&arena, &(result.analysis), (ANALYSIS))
#define IF_NOT_EQUALS(RESULT, EVALUATING, FIELD)                               \
  do {                                                                         \
    if (!EVALUATING.are_equals) {                                              \
      RESULT.are_equals = false;                                               \
      analysis_append(EVALUATING);                                             \
    }                                                                          \
  } while (0)

ResultEquals let_statement_equals(LetStatement *a, LetStatement *b);
ResultEquals return_statement_equals(ReturnStatement *a, ReturnStatement *b);
ResultEquals expression_equals(Expression *a, Expression *b);
ResultEquals node_equals(Node *a, Node *b);
ResultEquals identifier_equals(Identifier, Identifier);
ResultEquals integer_literal_equals(IntLiteral, IntLiteral);
ResultEquals boolean_equals(Boolean, Boolean);
ResultEquals prefix_equals(PrefixExpression, PrefixExpression);
ResultEquals infix_equals(InfixExpression, InfixExpression);
ResultEquals if_expression_equals(IfExpression, IfExpression);
ResultEquals function_literal_equals(FunctionLiteral, FunctionLiteral);
ResultEquals function_call_equals(FunctionCallExpression,
                                  FunctionCallExpression);
ResultEquals block_statement_equals(BlockStatement, BlockStatement);
ResultEquals test_token_equals(Token, Token);

bool test_object_integer(Object, I64);
bool test_object_bool(Object, bool);

Node *new_node(char *input);
void test_print_expression(Expression *expression);
#endif // !_TEST_H
