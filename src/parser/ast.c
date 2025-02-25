#include <stdio.h>
#ifndef _PARSER_H
#include "../arena.c"
#include "../arena_strings.c"
#include "../array.c"
#include "../lexer.c"
#include "../token.c"
#define color(C) "\033[0;3" #C "m"
#define end_color "\033[0m"
#define SYNTAX_ERROR color(3) "[SYNTAX ERROR] : " end_color
#define PARSING_ERROR color(3) "[PARSING ERROR] : " end_color
#define cast_statement(STMNT, TYPE) *(TYPE *)STMNT.data
#define cast(THIS, TYPE) *(TYPE *)THIS
typedef long long I64;

// TODO: Refactor all void * fields in structs to be named `data` or raw_bytes
// or just bytes, cause it makes everything more comfortable
typedef enum {
  NIL_STATEMENT,
  LET_STATEMENT,
  RETURN_STATEMENT,
  EXPRESSION_STATEMENT,
  // expressions
  IDENTIFIER_EXP,
  INTEGER_LIT_EXP,
  BOOLEAN_EXP,
  PREFIX_EXP,
  INFIX_EXP,
} NodeType;

// When we are parsing an expression we dont know the Precedence we have with it
// but when we are at certains tokens we KNOW which is,
// for example when we are parsing a prefix thing we know it has a precedence of
// PREFIX_PREC
typedef enum {
  NIL_PREC,          // 0
  LOWEST_PREC,       // 1
  EQUALS_PREC,       // == 2
  LESS_GREATER_PREC, // > or < 3
  SUM_PREC,          // + 4
  PRODUCT_PREC,      // * 5
  PREFIX_PREC,       // -X or !X 6
  CALL_PREC,         // myFunction(X) 7
} Precedence;

// Individual parts of expressions
//
// TODO: ver como puedo en vez de tener que usar un void * poder tener toda la
// informacion aqui en discovery kids, porque, no me gusta tener o sea si, es
// una arena collected language, pero aun asi, queria mantener la menor cantidad
typedef struct {
  NodeType type;
  void *exp_bytes;
} Expression;

typedef struct {
  Token token;
  I64 value;
} IntLiteral;

// this can be an expression or a part of a statement
// let foo = 1; <- this is an statement
// foo;         <- this is an expression
typedef struct {
  Token token;
  String value;
} Identifier;

typedef struct {
  Token token;
  bool value;
} Boolean;

// statements
typedef struct {
  Token token;
  NodeType type;
  void *data;
} Node;

typedef struct {
  Token token;
  Identifier name; // removed *
  Expression expression_value;
} LetStatement;

typedef struct {
  Token token;
  Expression expression_value;
} ReturnStatement;

typedef struct {
  Token token;
  String operator;
  Expression right;
} PrefixExpression;

typedef struct {
  Token token;
  Expression left;
  String operator;
  Expression right;
} InfixExpression;

// TODO:
// maybe i should just remove this to only
// be the Expression struct because it confuses me
// a LOT
typedef struct {
  Token token; // first token of the expression
  Expression expression_value;
} ExpressionStatement;

typedef struct {
  Node *statements;
} Program;

typedef struct {
  String error;
} Error;

typedef struct {
  Lexer *lexer;
  Error *errors;
  Token curr_token;
  Token peek_token;
} Parser;

typedef struct {
  TokenType key;
  void *value;
  bool is_occupied;
} KeyValue_PrefixFNS;

typedef Expression (*prefix_parse_fn)(Arena *, Parser *);
typedef Expression (*infix_parse_fn)(Arena *, Parser *, Expression);

#define kv_p(K, V)                                                             \
  (KeyValue_PrefixFNS) { .key = K, .value = V, .is_occupied = true }

typedef struct {
  prefix_parse_fn prefix_fn;
  infix_parse_fn infix_fn;
} ParsingFunctions;

typedef struct {
  TokenType key;
  ParsingFunctions value;
  bool is_occupied;
} KeyValue_PF;

typedef struct {
  TokenType key;
  Precedence value;
  bool is_occupied;
} KeyValue_PRC;

bool compare_token_type_keys(void *a, void *b) {
  KeyValue_PF a_key = *(KeyValue_PF *)a;
  TokenType b_key = *(TokenType *)b;
  return a_key.key == b_key;
}

// this one kinda sucks but i wont be using
// it because yeah i dont think is really necesary
bool compare_parsing_fns_values(void *a, void *b) {
  KeyValue_PF a_key = *(KeyValue_PF *)a;
  ParsingFunctions b_key = *(ParsingFunctions *)b;
  return a_key.value.infix_fn == b_key.infix_fn;
}

// fn defs
void ast_next_token(Arena *arena, Parser *parser);

void print_program(Arena *arena, Program *program);
void print_statement(Arena *arena, Node node);
String arena_stringify_statement(Arena *arena, Node node);

// parsing stuff
Parser ast_new_parser(Arena *arena, Lexer *lexer);
Program ast_parse_program(Arena *arena, Parser *parser);

Node *ast_parse_statement(Arena *arena, Parser *parser);
Node *ast_parse_let_statement(Arena *arena, Parser *parser);
Node *ast_parse_return_statement(Arena *arena, Parser *parser);
Node *ast_parse_expression_statement(Arena *arena, Parser *parser);
Expression ast_parse_identifier(Arena *arena, Parser *parser);
Expression ast_parse_boolean(Arena *arena, Parser *parser);
Expression ast_parse_int(Arena *arena, Parser *parser);
Expression ast_parse_prefix_expression(Arena *arena, Parser *parser);
Expression ast_parse_infix_expression(Arena *arena, Parser *parser,
                                      Expression left);
Expression ast_parse_expression(Arena *arena, Parser *parser, Precedence prece);
// utility
bool ast_expect_peek_token(Arena *arena, Parser *parser,
                           TokenType expected_type);
void ast_parser_peek_error(Arena *arena, Parser *parser,
                           TokenType expected_type);
prefix_parse_fn get_prefix_fn_from_hm(HashTable table, TokenType key);
infix_parse_fn get_infix_fn_from_hm(HashTable table, TokenType key);
String stringify_expression(Arena *arena, Node node, Expression expression);

#define ast_token_literal(NODE) (((Token *)NODE)->literal)

#define prs_fn(PREFIX, INFIX)                                                  \
  (ParsingFunctions) { .prefix_fn = PREFIX, .infix_fn = INFIX }
KeyValue_PF FUNCTIONS_ARR[] = {
    kv(KeyValue_PF, ILLEGAL, prs_fn(NULL, NULL)), // parsing stuff
    kv(KeyValue_PF, EOF_, prs_fn(NULL, NULL)),    //
    kv(KeyValue_PF, IDENTIFIER,
       prs_fn(&ast_parse_identifier, &ast_parse_infix_expression)), //
    kv(KeyValue_PF, INT, prs_fn(&ast_parse_int, NULL)),             //
    kv(KeyValue_PF, ASSIGN, prs_fn(NULL, NULL)),                    //
    kv(KeyValue_PF, MINUS,
       prs_fn(&ast_parse_prefix_expression, &ast_parse_infix_expression)), //
    kv(KeyValue_PF, PLUS,
       prs_fn(&ast_parse_prefix_expression, &ast_parse_infix_expression)), //
    kv(KeyValue_PF, ASTERISK,
       prs_fn(&ast_parse_prefix_expression, &ast_parse_infix_expression)), //
    kv(KeyValue_PF, SLASH,
       prs_fn(&ast_parse_prefix_expression, &ast_parse_infix_expression)), //
    kv(KeyValue_PF, EQUALS,
       prs_fn(&ast_parse_prefix_expression, &ast_parse_infix_expression)), //
    kv(KeyValue_PF, NOT_EQUALS,
       prs_fn(&ast_parse_prefix_expression, &ast_parse_infix_expression)), //
    kv(KeyValue_PF, BANG,
       prs_fn(&ast_parse_prefix_expression, &ast_parse_infix_expression)), //
    kv(KeyValue_PF, LT,
       prs_fn(&ast_parse_prefix_expression, &ast_parse_infix_expression)), //
    kv(KeyValue_PF, GT,
       prs_fn(&ast_parse_prefix_expression, &ast_parse_infix_expression)), //
    kv(KeyValue_PF, LT, prs_fn(NULL, NULL)),                               //
    kv(KeyValue_PF, GT, prs_fn(NULL, NULL)),                               //
    kv(KeyValue_PF, G_EQUALS, prs_fn(NULL, NULL)),                         //
    kv(KeyValue_PF, L_EQUALS, prs_fn(NULL, NULL)),                         //
    kv(KeyValue_PF, COMMA, prs_fn(NULL, NULL)),     // delimiters
    kv(KeyValue_PF, SEMICOLON, prs_fn(NULL, NULL)), //
    kv(KeyValue_PF, L_PAREN, prs_fn(NULL, NULL)),   //
    kv(KeyValue_PF, R_PAREN, prs_fn(NULL, NULL)),   //
    kv(KeyValue_PF, L_BRACE, prs_fn(NULL, NULL)),   //
    kv(KeyValue_PF, R_BRACE, prs_fn(NULL, NULL)),   //
    kv(KeyValue_PF, FUNCTION, prs_fn(NULL, NULL)),  //
    kv(KeyValue_PF, LET, prs_fn(NULL, NULL)),       //
    kv(KeyValue_PF, IF, prs_fn(NULL, NULL)),        //
    kv(KeyValue_PF, ELSE, prs_fn(NULL, NULL)),      //
    kv(KeyValue_PF, TRUE,
       prs_fn(&ast_parse_boolean, &ast_parse_infix_expression)), //
    kv(KeyValue_PF, FALSE,
       prs_fn(&ast_parse_boolean, &ast_parse_infix_expression)), //
    kv(KeyValue_PF, RETURN, prs_fn(NULL, NULL)),                 //
}; //

#define PARSING_FUNCTIONS_LEN (sizeof FUNCTIONS_ARR / sizeof FUNCTIONS_ARR[0])
HashTable PARSING_FUNCTIONS = (HashTable){
    .len = PARSING_FUNCTIONS_LEN,
    .item_size = sizeof(KeyValue_PF),
    .capacity = PARSING_FUNCTIONS_LEN,
    .items = FUNCTIONS_ARR,
    .are_keys_equals = &compare_token_type_keys,
    .are_values_equals = &compare_parsing_fns_values //
};

KeyValue_PRC PRECEDENCES_ARR[] = {
    kv(KeyValue_PRC, EQUALS, EQUALS_PREC),     //
    kv(KeyValue_PRC, NOT_EQUALS, EQUALS_PREC), //
    kv(KeyValue_PRC, LT, LESS_GREATER_PREC),   //
    kv(KeyValue_PRC, GT, LESS_GREATER_PREC),   //
    kv(KeyValue_PRC, MINUS, SUM_PREC),         //
    kv(KeyValue_PRC, PLUS, SUM_PREC),          //
    kv(KeyValue_PRC, ASTERISK, PRODUCT_PREC),  //
    kv(KeyValue_PRC, SLASH, PRODUCT_PREC),     //
};

#define new_hash_table(KV_TYPE, ARR, KEYS_EQ_FN, VALS_EQ_FN)                   \
  (HashTable) {                                                                \
    .len = array_len(ARR), .item_size = sizeof(KV_TYPE),                       \
    .capacity = array_len(ARR), .items = ARR, .are_keys_equals = KEYS_EQ_FN,   \
    .are_values_equals = VALS_EQ_FN                                            \
  }

HashTable PRECENDENCES = new_hash_table(KeyValue_PRC,             //
                                        PRECEDENCES_ARR,          //
                                        &compare_token_type_keys, //
                                        NULL                      //
);

Precedence get_precedence_from_hm(HashTable table, TokenType key) {
  KeyValue_PRC *kv = hash_table_find_item(table, &key);
  if (kv) {
    return kv->value;
  } else {
    return LOWEST_PREC;
  }
}

#define peek_precedence(PARSER)                                                \
  get_precedence_from_hm(PRECENDENCES, PARSER->peek_token.type)

#define curr_precedence(PARSER)                                                \
  get_precedence_from_hm(PRECENDENCES, PARSER->curr_token.type)

prefix_parse_fn get_prefix_fn_from_hm(HashTable table, TokenType key) {
  KeyValue_PF *kv = hash_table_find_item(table, &key);
  if (kv) {
    return kv->value.prefix_fn;
  } else {
    return NULL;
  }
}

infix_parse_fn get_infix_fn_from_hm(HashTable table, TokenType key) {
  KeyValue_PF *kv = hash_table_find_item(table, &key);
  if (kv) {
    return kv->value.infix_fn;
  } else {
    return NULL;
  }
}

Parser ast_new_parser(Arena *arena, Lexer *lexer) {
  Error *errors = arena_array(arena, Error);
  Parser parser = (Parser){.lexer = lexer,
                           .errors = errors,
                           .curr_token = (Token){0},
                           .peek_token = (Token){0}};
  // we do this twice because the read token func just makes the curr token the
  // same as the peek token and then calls lexer_next_token to asign it to
  // peek_token
  ast_next_token(arena, &parser);
  ast_next_token(arena, &parser);
  return parser;
}

void ast_next_token(Arena *arena, Parser *parser) {
  parser->curr_token = parser->peek_token;
  parser->peek_token = lexer_next_token(arena, parser->lexer);
}

// E es de Expected xd
#define peek_token_is(P, E) (P->peek_token.type == E)
#define curr_token_is(P, E) (P->curr_token.type == E)

// read the next token if it is the expected one
bool ast_expect_peek_token(Arena *arena, Parser *parser,
                           TokenType expected_type) {
  if (peek_token_is(parser, expected_type)) {
    ast_next_token(arena, parser);
    return true;
  } else {
    ast_parser_peek_error(arena, parser, expected_type);
    return false;
  }
}

#define error(MSG)                                                             \
  (Error) { .error = MSG }

void ast_parser_error_append(Parser *parser, String error_msg) {
  append(parser->errors, error(error_msg));
}

void ast_parser_peek_error(Arena *arena, Parser *parser,
                           TokenType expected_type) {
  char tmp_buf[1024];
  const char *expected_type_c_string = get_token_literal(expected_type);
  sprintf(tmp_buf,
          SYNTAX_ERROR "expected token to be : " color(
              6) "`%s`" end_color " got instead : " color(3) "`%s`" end_color,
          expected_type_c_string, parser->peek_token.literal.str);
  String error_msg = arena_new_string(arena, tmp_buf);
  append(parser->errors, error(error_msg));
}

Node *ast_parse_dummy_statement(Arena *arena, Parser *parser) {
  Node *statement = arena_alloc(arena, sizeof(Node));
  statement->token = parser->curr_token;
  statement->type = NIL_STATEMENT;
  statement->data = NULL;
  return statement;
}

// TODO: FIND A WAY TO PUT THIS IN A MACRO
/*
 Node *statement = arena_alloc(arena, sizeof(Node));
ReturnStatement *let_statement = arena_alloc(arena, sizeof(ReturnStatement));
statement->type = RETURN_STATEMENT;
statement->data = let_statement;
let_statement->token = parser->curr_token;
*/

// LET_STATEMENT
Node *ast_parse_let_statement(Arena *arena, Parser *parser) {
  Node *statement = arena_alloc(arena, sizeof(Node));
  LetStatement *let_statement = arena_alloc(arena, sizeof(LetStatement));
  statement->type = LET_STATEMENT;
  statement->data = let_statement;
  let_statement->token = parser->curr_token;
  // reads the next token IF IT IS the expected one
  if (ast_expect_peek_token(arena, parser, IDENTIFIER)) {
    let_statement->name = (Identifier){.token = parser->curr_token,
                                       .value = parser->curr_token.literal};
  }

  // reads the next token IF IT IS the expected one
  if (!ast_expect_peek_token(arena, parser, ASSIGN)) {
    // here we should pop from the arena rite?
    return NULL;
  }
  // SILLY : Silliness warning this was made just to try something, do not keep
  // it like this!
  ast_next_token(arena, parser);

  // TODO: PARSE EXPRESSIONS (this is just for a test, i gotta make this really
  // work later)
  let_statement->expression_value =
      ast_parse_expression(arena, parser, LOWEST_PREC);
  while (!(curr_token_is(parser, SEMICOLON))) {
    ast_next_token(arena, parser);
  }
  return statement;
}

Node *ast_parse_return_statement(Arena *arena, Parser *parser) {
  Node *statement = arena_alloc(arena, sizeof(Node));
  ReturnStatement *let_statement = arena_alloc(arena, sizeof(ReturnStatement));
  statement->type = RETURN_STATEMENT;
  statement->data = let_statement;
  let_statement->token = parser->curr_token;

  // TODO: PARSE EXPRESSIONS

  while (!(curr_token_is(parser, SEMICOLON))) {
    ast_next_token(arena, parser);
  }

  return statement;
}

// IDENTIFIER_LIT_EXP
Expression ast_parse_int(Arena *arena, Parser *parser) {
  IntLiteral *integer_literal = arena_alloc(arena, sizeof(IntLiteral));
  integer_literal->token = parser->curr_token;
  // TODO: add error handling to this function cause it
  // may fail so yeah but right now we dont care!
  // Result<Int,Error>
  integer_literal->value = string_to_integer_64(parser->curr_token.literal);
  return (Expression){.type = INTEGER_LIT_EXP,
                      .exp_bytes = (void *)integer_literal};
}

Expression ast_parse_identifier(Arena *arena, Parser *parser) {
  (void)arena;
  (void)parser;
  Identifier *identifier = arena_alloc(arena, sizeof(Identifier));
  identifier->token = parser->curr_token;
  identifier->value = parser->curr_token.literal;
  return (Expression){.type = IDENTIFIER_EXP, .exp_bytes = (void *)identifier};
}

Expression ast_parse_boolean(Arena *arena, Parser *parser) {
  (void)arena;
  (void)parser;
  Boolean *boolean = arena_alloc(arena, sizeof(Boolean));
  boolean->token = parser->curr_token;
  boolean->value = curr_token_is(parser, TRUE);
  return (Expression){.type = BOOLEAN_EXP, .exp_bytes = (void *)boolean};
}

// ALL THE OTHER EXPRESSIONS

Expression ast_parse_infix_expression(Arena *arena, Parser *parser,
                                      Expression left) {
  InfixExpression *infix_exp = arena_alloc(arena, sizeof(InfixExpression));
  infix_exp->token = parser->curr_token;
  infix_exp->operator= parser->curr_token.literal;
  infix_exp->left = left;
  Precedence precedence =
      get_precedence_from_hm(PRECENDENCES, parser->curr_token.type);
  ast_next_token(arena, parser);
  infix_exp->right = ast_parse_expression(arena, parser, precedence);
  Expression resulting_exp =
      (Expression){.type = INFIX_EXP, .exp_bytes = (void *)infix_exp};
  return resulting_exp;
}

Expression ast_parse_prefix_expression(Arena *arena, Parser *parser) {
  PrefixExpression *prefix_exp = arena_alloc(arena, sizeof(PrefixExpression));
  prefix_exp->token = parser->curr_token;
  prefix_exp->operator= parser->curr_token.literal;
  ast_next_token(arena, parser);
  prefix_exp->right = ast_parse_expression(arena, parser, PREFIX_PREC);
  return (Expression){.type = PREFIX_EXP, .exp_bytes = (void *)prefix_exp};
}

// TODO:
// HOW do we recover from error here
// gamer idea just return a pointer dude, the problem
// is that would require to allocate in the heap and
// we are divorced, an empty struct will do the trick
// maybe a expression nil or something like that
// like the one randys did in the game dev videos
// esto es super cool, el left_value no se pierde o se borra porque
// siempre me lo paso al infix, y ahi lo asigno como left_value
// que chido no
Expression ast_parse_expression(Arena *arena, Parser *parser,
                                Precedence precedence) {
  prefix_parse_fn prefix =
      get_prefix_fn_from_hm(PARSING_FUNCTIONS, parser->curr_token.type);
  if (!prefix) {
    String error = arena_string_fmt(
        arena, PARSING_ERROR "Not prefix function found for: %s",
        parser->curr_token.literal.str,
        get_token_literal(parser->curr_token.type));
    ast_parser_error_append(parser, error);
    return (Expression){0};
  }

  Expression left_value = prefix(arena, parser);

  while (!peek_token_is(parser, SEMICOLON) &&
         precedence < peek_precedence(parser)) {
    infix_parse_fn infix =
        get_infix_fn_from_hm(PARSING_FUNCTIONS, parser->peek_token.type);

    if (!infix) {
      return left_value;
    }

    ast_next_token(arena, parser);
    left_value = infix(arena, parser, left_value);
  }

  return left_value;
}

Node *ast_parse_expression_statement(Arena *arena, Parser *parser) {
  Node *statement = arena_alloc(arena, sizeof(Node));
  Token initial_token = parser->curr_token;

  ExpressionStatement *expr_statement =
      arena_alloc(arena, sizeof(ExpressionStatement));

  expr_statement->expression_value =
      ast_parse_expression(arena, parser, LOWEST_PREC);

  statement->token = initial_token;
  statement->type = EXPRESSION_STATEMENT;
  statement->data = expr_statement;

  if (peek_token_is(parser, SEMICOLON)) {
    ast_next_token(arena, parser);
  }

  return statement;
}

Node *ast_parse_statement(Arena *arena, Parser *parser) {
  Token curr_token = parser->curr_token;
  switch (curr_token.type) {
  case LET:
    //
    {
      return ast_parse_let_statement(arena, parser);
    }
  case RETURN:
    //
    {
      return ast_parse_return_statement(arena, parser);
    }
  default:
    //
    {
      return ast_parse_expression_statement(arena, parser);
    }
  }
}

Program ast_parse_program(Arena *arena, Parser *parser) {
  Node *statements = arena_array(arena, Node);
  while (parser->curr_token.type != EOF_) {
    Node *stmnt = ast_parse_statement(arena, parser);
    if (stmnt) {
      append(statements, *stmnt);
    }
    ast_next_token(arena, parser);
  }
  return (Program){.statements = statements};
}

String ast_init(Program *program) {
  if (len(program->statements)) {
    return ast_token_literal(&program->statements[0]);
  } else {
    return string("");
  }
}

void print_program(Arena *arena, Program *program) {
  printf(color(6) "[PROGRAM] \n" end_color);
  for (size_t i = 0; i < len(program->statements); i++) {
    print_statement(arena, program->statements[i]);
  }
}

void print_parser_errors(Parser parser) {
  for (size_t i = 0; i < len(parser.errors); i++) {
    Error err = parser.errors[i];
    printfln("%S\n", err.error);
  }
}

String stringify_program(Arena *arena, Program *program) {
  String result = arena_new_empty_string_with_cap(arena, 256);
  for (size_t i = 0; i < len(program->statements); i++) {
    string_concat_resize(
        arena, &result,
        arena_stringify_statement(arena, program->statements[i]));
  }
  return result;
}

String stringify_expression(Arena *arena, Node node, Expression expression) {
  (void)node;
  String exp_string = {0};
  switch (expression.type) {
  case IDENTIFIER_EXP: {
    Identifier parsed_identifier = *(Identifier *)expression.exp_bytes;
    exp_string =
        arena_string_fmt(arena, "%s", parsed_identifier.token.literal.str);
    break;
  }
  case INTEGER_LIT_EXP: {
    IntLiteral parsed_int = *(IntLiteral *)expression.exp_bytes;
    // shit explodes here huhu it overwrites the bang memory for some reason
    // good luck!
    exp_string = arena_string_fmt(arena, "%d", parsed_int);
    /*exp_string = string("5");*/
    break;
  }
  case PREFIX_EXP: {
    PrefixExpression prefix = cast(expression.exp_bytes, PrefixExpression);
    String operator= prefix.operator;
    String right = stringify_expression(arena, node, prefix.right);
    exp_string = arena_string_fmt(arena, "(%s%s)", operator.str, right.str);
    break;
  }
  case INFIX_EXP: {
    InfixExpression infix = cast(expression.exp_bytes, InfixExpression);
    String left = stringify_expression(arena, node, infix.left);
    String operator= infix.operator;
    String right = stringify_expression(arena, node, infix.right);
    exp_string = arena_string_fmt(arena, "(%s %s %s)", left.str, operator.str,
                                  right.str);
    break;
  }
  default: {
    // TODO:
    // make the stupid macro that makes string representations
    // for enums too please
    exp_string = arena_string_fmt(
        arena,
        "There has not been a expression string for this expression type: %d",
        expression.type);
  }
  }
  return exp_string;
}

String arena_stringify_statement(Arena *arena, Node node) {
  String str_stmt;
  switch (node.type) {
  case LET_STATEMENT: {
    LetStatement statement = *((LetStatement *)node.data);
    String expression_string =
        stringify_expression(arena, node, statement.expression_value);
    str_stmt = arena_string_fmt(     //
        arena,                       //
        "%s %s = %s",                //
        statement.token.literal.str, //
        statement.name.value.str,    //
        expression_string.str        //
    );
    break;
  }
  case RETURN_STATEMENT:
    //
    {
      ReturnStatement statement = *((ReturnStatement *)node.data);
      String expression_string =
          stringify_expression(arena, node, statement.expression_value);
      str_stmt = arena_string_fmt(arena, "%s %s",              //
                                  statement.token.literal.str, //
                                  expression_string.str        //
      );
      break;
    }
  case EXPRESSION_STATEMENT:
    //
    {
      ExpressionStatement statement = *((ExpressionStatement *)node.data);
      String expression_string =
          stringify_expression(arena, node, statement.expression_value);
      str_stmt = arena_string_fmt(arena, "%s",          //
                                  expression_string.str //
      );
      break;
    }
  default:
    //
    {
      str_stmt = arena_string_fmt(
          arena,
          "ILLEGAL STUFF IDUNNO BRO THERE IS NOT THAT STATEMENT" //
      );
    }
  }
  return str_stmt;
}

void print_statement(Arena *arena, Node node) {
  String statement = arena_stringify_statement(arena, node);
  printf("%s\n", statement.str);
}

/*void print_expression(Expression expression) {*/
/*  switch (expression.type) {*/
/*    case :*/
/*  }*/
/*}*/

#endif /* ifndef _PARSER_H */
