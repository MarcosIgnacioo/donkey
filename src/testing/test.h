#include "../array.c"
#include "../lexer.c"
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

bool _expression_equals(Expression *exp_a, Expression *exp_b) {
  bool are_equal = true;
  assert(exp_a->type == INFIX_EXP);
  assert(exp_b->type == INFIX_EXP);
  InfixExpression infix_a = exp_a->infix;
  InfixExpression infix_b = exp_b->infix;

  if (!token_equals(infix_a.token, infix_b.token)) {
    are_equal = false;
    print_error("Tokens are not equal");
  }

  if (!_expression_equals(infix_a.left, infix_b.left)) {
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
} ExpressionEquals;

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

ExpressionEquals expression_equals(Expression *a, Expression *b) {
  ExpressionEquals result = {.are_equals = true,
                             .analysis =
                                 arena_new_empty_string_with_cap(&arena, 512)};

  // this would be an amazing use of a temp arena btw maybe in a future refactor
  String error = {0};
  if (a->type != b->type) {
    result.are_equals = false;
    error = arena_string_fmt(&arena,
                             "(a->type:%s) != (b->type:%s)", //
                             NODE_TYPES[a->type],            //
                             NODE_TYPES[b->type]             //
    );
    analysis_append(error);
  }
  return result;
}

Expression *new_expression(char *input) {
  Lexer lexer = lexer_new_lexer(string(input));
  Parser parser = ast_new_parser(&arena, &lexer);
  Expression *expression = ast_parse_expression(&arena, &parser, LOWEST_PREC);
  return expression;
}

void test_print_expression(Expression *expression) {
  String expression_string = stringify_expression(&arena, NULL, expression);
  printf("%s\n", expression_string.str);
}

void test() {
  Expression *a = new_expression("1 + 2");
  Expression *b = new_expression("++2");
  ExpressionEquals res = expression_equals(a,b);
  if (res.are_equals) {
    printf(LOG_SUCCESS"everything stays\n");
  } else {
    printf(LOG_ERROR"%s\n", res.analysis.str);
  }
}
