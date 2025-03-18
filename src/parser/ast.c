#ifndef _PARSER_H
#define _PARSER_H
char BURRO[38][213] = {
    "                                                                       ",
    "                     #############     ###########                     ",
    "                    ######        ###         ######                   ",
    "                    ### ########        #######  ###                   ",
    "                   ### ####                  #### ###                  ",
    "                   ## ##                       ### ##                  ",
    "                   ## # ######   #    ##  ###### # ##                  ",
    "                   ######   ##  #      # ###   # # ##                  ",
    "                  ## # # #### # ##    ## # #### # ###                  ",
    "                   #   ####    ###    ###  # ####  ##                  ",
    "                   ###    # #####      ##### # ##  ##                  ",
    "                   ############         #############                  ",
    "                  ###########           #  ###########                 ",
    "                 ### ##                          ## ###                ",
    "                ####           ##########           ####               ",
    "                ###        ##################        ###               ",
    "               ###       ####   ##       ######       ##               ",
    "               ###      ##                    ##      ###              ",
    "               ##      #                        #     ###              ",
    "               ##        #######       ########  #     ##              ",
    "              ###       #######    ##    #######       ###             ",
    "              ##       #                  #     #      ###             ",
    "             ###                         #      #       ##             ",
    "             ###      #        ##      ##               ##             ",
    "             ###                   ##                   ###            ",
    "             ###   #                                    ###            ",
    "             ###                                        ###            ",
    "             ###   ##                             ###   ##             ",
    "              ###  ###                            ###   ##             ",
    "              ### #####                          ##### ###             ",
    "               ###################################### ###              ",
    "                ##    ###     ###########      ###  # ##               ",
    "                  #        ########   #######                          ",
    "                          ####################                         ",
    "                             ######     ##                             ",
    "                                                                       ",
    "                   ###                             ##                  ",
    "                                                                       "};

#include "../arena.c"
#include "../arena_strings.c"
#include "../array.c"
#include "../lexer.c"
#include "../token.c"
#include <stdio.h>
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
  STRING_LIT_EXP,
  ARRAY_EXP,
  BOOLEAN_EXP,
  PREFIX_EXP,
  INFIX_EXP,
  IF_EXP,
  FUNCTION_LITERAL_EXP,
  FUNCTION_CALL_EXP,
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
  PAREN_PREC,        // () 7
  CALL_PREC,         // myFunction(X) 8
} Precedence;

// Individual parts of expressions
//
// TODO: ver como puedo en vez de tener que usar un void * poder tener toda la
// informacion aqui en discovery kids, porque, no me gusta tener o sea si, es
// una arena collected language, pero aun asi, queria mantener la menor cantidad
/*typedef struct {*/
/*  NodeType type;*/
/*  void *exp_bytes;*/
/*} Expression;*/
typedef struct Expression Expression;

typedef struct {
  Token token;
  I64 value;
} IntLiteral;

typedef struct {
  Token token;
  String value;
} StringLiteral;

typedef struct {
  Token token;
  Expression **value;
} Array;

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
  Identifier name; // removed *
  Expression *expression_value;
} LetStatement;

typedef struct {
  Token token;
  Expression *expression_value;
} ReturnStatement;

typedef struct {
  Token token; // first token of the expression
  Expression *expression_value;
} ExpressionStatement;

typedef struct {
  Token token;
  NodeType type;
  union {
    LetStatement let_statement;
    ReturnStatement return_statement;
    ExpressionStatement expression_statement;
  };
} Node;

// TOREFACTOR: put block statements as a part of a union node and well refactor
// girlllllllllllll
typedef struct {
  Node *statements;
} BlockStatement;

typedef struct {
  Token token; // the if bro
  Expression *condition;
  BlockStatement consequence;
  BlockStatement alternative;
} IfExpression;

typedef struct {
  Token token; // the fn bro
  Identifier name;
  Identifier *parameters;
  BlockStatement body;
} FunctionLiteral;

typedef struct {
  Token token; // the ( token
  Expression **arguments;
  Expression *function;
} FunctionCallExpression;

typedef struct {
  Token token;
  String operator;
  Expression *right;
} PrefixExpression;

typedef struct {
  Token token;
  Expression *left;
  String operator;
  Expression *right;
} InfixExpression;

struct Expression {
  NodeType type;
  union {
    InfixExpression infix;
    PrefixExpression prefix;
    Identifier identifier;
    IntLiteral integer_literal;
    StringLiteral string_literal;
    Array array;
    IfExpression if_expression;
    FunctionCallExpression function_call;
    FunctionLiteral function_literal;
    Boolean boolean;
  };
};
// REFACTOR: incoming object oriented programming into c

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

typedef Expression *(*prefix_parse_fn)(Arena *, Parser *);
typedef Expression *(*infix_parse_fn)(Arena *, Parser *, Expression *);

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
void print_statement(Arena *arena, Node *node);
String arena_stringify_statement(Arena *arena, Node *node);

// parsing stuff
Parser ast_new_parser(Arena *arena, Lexer *lexer);
Program ast_parse_program(Arena *arena, Parser *parser);
BlockStatement ast_parse_block_statement(Arena *arena, Parser *parser);

Node *ast_parse_statement(Arena *arena, Parser *parser);
Node *ast_parse_let_statement(Arena *arena, Parser *parser);
Node *ast_parse_return_statement(Arena *arena, Parser *parser);
Expression *ast_parse_expression(Arena *arena, Parser *parser,
                                 Precedence prece);
Node *ast_parse_expression_statement(Arena *arena, Parser *parser);
Expression *ast_parse_identifier(Arena *arena, Parser *parser);
Expression *ast_parse_boolean(Arena *arena, Parser *parser);
Expression *ast_parse_int(Arena *arena, Parser *parser);
Expression *ast_parse_string(Arena *arena, Parser *parser);
Expression *ast_parse_grouped_expression(Arena *arena, Parser *parser);
Expression *ast_parse_array(Arena *arena, Parser *parser);
Expression *ast_parse_array_indexing(Arena *arena, Parser *parser,
                                     Expression *left);
Expression *ast_parse_if_expression(Arena *arena, Parser *parser);
Expression *ast_parse_function_literal(Arena *arena, Parser *parser);
Identifier *ast_parse_function_parameters(Arena *arena, Parser *parser);
Expression **ast_parse_call_function_arguments(Arena *arena, Parser *parser);
Expression **ast_parse_array_members(Arena *arena, Parser *parser);

Expression *ast_parse_function_call_expression(Arena *arena, Parser *parser,
                                               Expression *expression);
Expression *ast_parse_prefix_expression(Arena *arena, Parser *parser);
Expression *ast_parse_infix_expression(Arena *arena, Parser *parser,
                                       Expression *left);
// utility
String arena_join_identifier_array(Arena *arena, Identifier *identifiers);
String join_expression_caster(Arena *arena, void *data);
String join_argument_caster(Arena *arena, void *data);
bool ast_expect_peek_token(Arena *arena, Parser *parser,
                           TokenType expected_type);
void ast_parser_peek_error(Arena *arena, Parser *parser,
                           TokenType expected_type);
bool ast_expect_curr_token(Arena *arena, Parser *parser,
                           TokenType expected_type);
void ast_parser_curr_error(Arena *arena, Parser *parser,
                           TokenType expected_type);
prefix_parse_fn get_prefix_fn_from_hm(HashTable table, TokenType key);
infix_parse_fn get_infix_fn_from_hm(HashTable table, TokenType key);
String stringify_expression(Arena *arena, Node *node, Expression *expression);

#define ast_token_literal(NODE) (((Token *)NODE)->literal)

#define prs_fn(PREFIX, INFIX)                                                  \
  (ParsingFunctions) { .prefix_fn = PREFIX, .infix_fn = INFIX }
KeyValue_PF FUNCTIONS_ARR[] = {
    kv(KeyValue_PF, ILLEGAL, prs_fn(NULL, NULL)), // parsing stuff
    kv(KeyValue_PF, EOF_, prs_fn(NULL, NULL)),    //
    kv(KeyValue_PF, IDENTIFIER,
       prs_fn(&ast_parse_identifier, &ast_parse_infix_expression)), //
    kv(KeyValue_PF, INT, prs_fn(&ast_parse_int, NULL)),             //
    kv(KeyValue_PF, STRING, prs_fn(&ast_parse_string, NULL)),       //
    kv(KeyValue_PF, ASSIGN,
       prs_fn(NULL, NULL)), // implement reasigning variables :3
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
    kv(KeyValue_PF, G_EQUALS, prs_fn(NULL, NULL)),                         //
    kv(KeyValue_PF, L_EQUALS, prs_fn(NULL, NULL)),                         //
    kv(KeyValue_PF, COMMA, prs_fn(NULL, NULL)),     // delimiters
    kv(KeyValue_PF, SEMICOLON, prs_fn(NULL, NULL)), //
    kv(KeyValue_PF, L_PAREN,
       prs_fn(&ast_parse_grouped_expression,
              &ast_parse_function_call_expression)), //
    kv(KeyValue_PF, R_PAREN, prs_fn(NULL, NULL)),    //
    kv(KeyValue_PF, L_BRACE, prs_fn(NULL, NULL)),    //
    kv(KeyValue_PF, R_BRACE, prs_fn(NULL, NULL)),    //
    kv(KeyValue_PF, L_SQUARE_BRACE,
       prs_fn(&ast_parse_array, &ast_parse_array_indexing)),              //
    kv(KeyValue_PF, R_SQUARE_BRACE, prs_fn(NULL, NULL)),                  //
    kv(KeyValue_PF, FUNCTION, prs_fn(&ast_parse_function_literal, NULL)), //
    kv(KeyValue_PF, LET, prs_fn(NULL, NULL)),                             //
    kv(KeyValue_PF, IF, prs_fn(&ast_parse_if_expression, NULL)),          //
    kv(KeyValue_PF, ELSE, prs_fn(NULL, NULL)),                            //
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
    kv(KeyValue_PRC, L_PAREN, CALL_PREC),      //
    /*kv(KeyValue_PRC, R_PAREN, PAREN_PREC),     //*/
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

#define new_identifier(parser)                                                 \
  (Identifier) {                                                               \
    .token = parser->curr_token, .value = parser->curr_token.literal,          \
  }

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

// expects dynamic array
String arena_join_identifier_array(Arena *arena, Identifier *identifiers) {
  String join = arena_new_empty_string_with_cap(arena, 512);
  for (I64 i = 0; i < len(identifiers); i++) {
    if (i > 0) {
      arena_string_concat(arena, &join, string(", "));
    }
    String arg = identifiers[i].value;
    string_concat(&join, arg);
  }
  return join;
}

String arena_join_expression_array(Arena *arena, Expression **expressions,
                                   String separator) {
  String join = arena_new_empty_string_with_cap(arena, 512);
  for (I64 i = 0; i < len(expressions); i++) {
    if (i > 0) {
      arena_string_concat(arena, &join, string(", "));
    }
    // TODO after REFACTOR 2 here i wont need to ampersand cause it will be a
    // pointers array
    String arg = stringify_expression(arena, NULL, expressions[i]);
    string_concat(&join, arg);
  }
  return join;
}

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

bool ast_expect_curr_token(Arena *arena, Parser *parser,
                           TokenType expected_type) {
  if (curr_token_is(parser, expected_type)) {
    ast_next_token(arena, parser);
    return true;
  } else {
    ast_parser_curr_error(arena, parser, expected_type);
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
          SYNTAX_ERROR "expected PEEK token to be : " color(
              6) "`%s`" end_color " got instead : " color(3) "`%s`" end_color,
          expected_type_c_string, parser->peek_token.literal.str);
  String error_msg = arena_new_string(arena, tmp_buf);
  append(parser->errors, error(error_msg));
}

void ast_parser_curr_error(Arena *arena, Parser *parser,
                           TokenType expected_type) {
  char tmp_buf[1024];
  const char *expected_type_c_string = get_token_literal(expected_type);
  sprintf(tmp_buf,
          SYNTAX_ERROR "expected CURR token to be : " color(
              6) "`%s`" end_color " got instead : " color(3) "`%s`" end_color,
          expected_type_c_string, parser->curr_token.literal.str);
  String error_msg = arena_new_string(arena, tmp_buf);
  append(parser->errors, error(error_msg));
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
  LetStatement let_statement = (LetStatement){0};
  let_statement.token = parser->curr_token;
  // reads the next token IF IT IS the expected one
  if (ast_expect_peek_token(arena, parser, IDENTIFIER)) {
    let_statement.name = (Identifier){.token = parser->curr_token,
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
  let_statement.expression_value =
      ast_parse_expression(arena, parser, LOWEST_PREC);

  if (peek_token_is(parser, SEMICOLON)) {
    ast_next_token(arena, parser);
  }

  statement->type = LET_STATEMENT;
  statement->let_statement = let_statement;
  return statement;
}

Node *ast_parse_return_statement(Arena *arena, Parser *parser) {
  Node *statement = arena_alloc(arena, sizeof(Node));
  ReturnStatement return_statement = (ReturnStatement){0};

  return_statement.token = parser->curr_token;
  ast_next_token(arena, parser);

  return_statement.expression_value =
      ast_parse_expression(arena, parser, LOWEST_PREC);

  if (peek_token_is(parser, SEMICOLON)) {
    ast_next_token(arena, parser);
  }

  statement->type = RETURN_STATEMENT;
  statement->return_statement = return_statement;
  return statement;
}

// IDENTIFIER_LIT_EXP
Expression *ast_parse_int(Arena *arena, Parser *parser) {
  (void)arena;
  (void)parser;
  IntLiteral integer_literal = (IntLiteral){0};
  integer_literal.token = parser->curr_token;
  // TODO: add error handling to this function cause it
  // may fail so yeah but right now we dont care!
  // Result<Int,Error>
  integer_literal.value = string_to_integer_64(parser->curr_token.literal);

  Expression *expression = arena_alloc(arena, sizeof(Expression));
  expression->type = INTEGER_LIT_EXP;
  expression->integer_literal = integer_literal;

  return expression;
}

Expression *ast_parse_string(Arena *arena, Parser *parser) {
  (void)arena;
  (void)parser;
  StringLiteral string_literal = (StringLiteral){0};
  string_literal.token = parser->curr_token;
  // TODO: add error handling to this function cause it
  // may fail so yeah but right now we dont care!
  // Result<Int,Error>
  string_literal.value = parser->curr_token.literal;

  Expression *expression = arena_alloc(arena, sizeof(Expression));
  expression->type = STRING_LIT_EXP;
  expression->string_literal = string_literal;

  return expression;
}

Expression *ast_parse_identifier(Arena *arena, Parser *parser) {
  (void)parser;

  Identifier identifier = (Identifier){0};
  identifier.token = parser->curr_token;
  identifier.value = parser->curr_token.literal;

  Expression *identifier_expression = arena_alloc(arena, sizeof(Expression));
  identifier_expression->type = IDENTIFIER_EXP;
  identifier_expression->identifier = identifier;

  return identifier_expression;
}

Expression *ast_parse_boolean(Arena *arena, Parser *parser) {
  (void)arena;
  (void)parser;
  Boolean boolean = (Boolean){0};
  boolean.token = parser->curr_token;
  boolean.value = curr_token_is(parser, TRUE);

  Expression *expression = arena_alloc(arena, sizeof(Expression));
  expression->type = BOOLEAN_EXP;
  expression->boolean = boolean;

  return expression;
}

// ALL THE OTHER EXPRESSIONS

// TODO: debug this to completly understand it
// basically when we parse an expression we get the parse_prefix_fn
// and parse the left value with that one, corresponding to specific tokens
// the `+` makes a PrefixExpression which is a {operator='+', right_value='1'};
// and the `(` makes a whole expression, with just the straight forward
// ast_parse_expression function, which will stop when it finds the ) because
// it doenst have precedence so it will have a 0 precedence, which will make
// everything bubble up until it gets to the outest caller of that where checks
// LOWEST_PREC < LOWEST_PREC which aint true so it just goes back up here and
// then we expect token read if it is ) if not return empty expression idk if
// returning empty is correct because it might loose the error msg but who knows
//
Expression *ast_parse_grouped_expression(Arena *arena, Parser *parser) {
  ast_next_token(arena, parser);
  Expression *exp = ast_parse_expression(arena, parser, LOWEST_PREC);
  if (!ast_expect_peek_token(arena, parser, R_PAREN)) {
    return NULL;
  }
  return exp;
}

Expression *ast_parse_array(Arena *arena, Parser *parser) {
  Token curr_token = parser->curr_token;
  Expression **array_members = ast_parse_array_members(arena, parser);
  Expression *array_expression = arena_alloc(arena, sizeof(Expression));
  array_expression->type = ARRAY_EXP;
  array_expression->array.token = curr_token;
  array_expression->array.value = array_members;
  /*ast_next_token(arena, parser);*/
  return array_expression;
}

Expression *ast_parse_array_indexing(Arena *arena, Parser *parser,
                                     Expression *left) {
  Token curr_token = parser->curr_token;
  Expression **array_members = ast_parse_array_members(arena, parser);
  Expression *array_expression = arena_alloc(arena, sizeof(Expression));
  array_expression->type = ARRAY_EXP;
  array_expression->array.token = curr_token;
  array_expression->array.value = array_members;
  /*ast_next_token(arena, parser);*/
  // TODO: this
  return NULL;
}

/*
when starting fn
if (1)
^  ^
   peek
curr

ast_expect_peek_token(arena, parser, L_PAREN)
(1
^^
 peek
curr

ast_next_token(arena,parser); // makes
1
^
peek
*/
Expression *ast_parse_if_expression(Arena *arena, Parser *parser) {
  Token if_token = parser->curr_token;
  Expression *condition;
  BlockStatement consequence, alternative;
  consequence = (BlockStatement){.statements = NULL};
  alternative = (BlockStatement){.statements = NULL};

  if (!ast_expect_peek_token(arena, parser, L_PAREN)) {
    return NULL;
  }

  ast_next_token(arena, parser); // reads to the next token,
                                 // from ( to the condition_exp
  condition = ast_parse_expression(arena, parser, LOWEST_PREC);

  if (!ast_expect_peek_token(arena, parser, R_PAREN)) {
    return NULL;
  }

  if (!ast_expect_peek_token(arena, parser, L_BRACE)) {
    return NULL;
  }

  consequence = ast_parse_block_statement(arena, parser);

  if (peek_token_is(parser, ELSE)) {
    ast_next_token(arena, parser);

    if (!ast_expect_peek_token(arena, parser, L_BRACE)) {
      return NULL;
    }
    alternative = ast_parse_block_statement(arena, parser);
  } else {
    alternative = (BlockStatement){.statements = NULL};
  }
  // why we dont do right brace parsing check hereee!!!!???? like
  // whyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy

  IfExpression if_expression = (IfExpression){
      .token = if_token,
      .condition = condition,
      .consequence = consequence,
      .alternative = alternative,
  };

  Expression *expression = arena_alloc(arena, sizeof(Expression));
  expression->type = IF_EXP;
  expression->if_expression = if_expression;

  return expression;
}

Identifier *mine_ast_parse_function_parameters(Arena *arena, Parser *parser) {
  Identifier *parameters = arena_array_with_cap(arena, Identifier, 16);

  if (peek_token_is(parser, R_PAREN)) {
    ast_next_token(arena, parser);
    return parameters;
  }

  Identifier tmp = (Identifier){0};

  // here we are like this at the beginin
  // (foo,bar)
  // ^
  // (foo,fuzz,bar)
  //  ^
  //  append(parameters,foo)
  //
  do {
    ast_next_token(arena, parser);
    tmp = new_identifier(parser);
    append(parameters, tmp);
    ast_next_token(arena, parser);
  } while (curr_token_is(parser, COMMA));

  ast_expect_curr_token(arena, parser, R_PAREN);

  return parameters;
  // this would always end the parser with the curr token being R_PAREN
}

Identifier *ast_parse_function_parameters(Arena *arena, Parser *parser) {
  Identifier *parameters = arena_array_with_cap(arena, Identifier, 16);

  if (peek_token_is(parser, R_PAREN)) {
    ast_next_token(arena, parser);
    return parameters;
  }

  ast_next_token(arena, parser);
  Identifier tmp = new_identifier(parser);
  append(parameters, tmp);

  while (peek_token_is(parser, COMMA)) {
    ast_next_token(arena, parser);
    ast_next_token(arena, parser);

    tmp = new_identifier(parser);
    append(parameters, tmp);
  }

  ast_expect_peek_token(arena, parser, R_PAREN);

  return parameters;
  // this would always end the parser with the curr token being R_PAREN
}

Expression *ast_parse_function_literal(Arena *arena, Parser *parser) {
  Token function_token = parser->curr_token;
  BlockStatement body;
  Identifier *parameters;
  Identifier name = (Identifier){0};
  body = (BlockStatement){.statements = NULL};
  /*fn(x, y) { return x + y; };*/

  if (peek_token_is(parser, L_PAREN)) {
    ast_next_token(arena, parser);
  } else if (ast_expect_peek_token(arena, parser, IDENTIFIER)) {
    name.token = parser->curr_token;
    name.value = parser->curr_token.literal;
    ast_expect_peek_token(arena, parser, L_PAREN);
  }

  parameters = ast_parse_function_parameters(arena, parser);

  ast_next_token(arena, parser);

  body = ast_parse_block_statement(arena, parser);

  FunctionLiteral function_literal = (FunctionLiteral){0};

  /* TODO:
     function_literal->name = new_identifier();
  */
  function_literal.token = function_token;

  function_literal.name = name;

  function_literal.parameters = parameters;
  function_literal.body = body;

  Expression *expression = arena_alloc(arena, sizeof(Expression));
  expression->type = FUNCTION_LITERAL_EXP;
  expression->function_literal = function_literal;

  return expression;
}

/*foo()*/
/*  infix_fn(arena, parser, left_value()== foo)*/
/*infix func*/
Expression *ast_parse_function_call_expression(Arena *arena, Parser *parser,
                                               Expression *function) {

  FunctionCallExpression function_call = (FunctionCallExpression){0};
  function_call.token = parser->curr_token; // the ( token
  function_call.function = function;
  function_call.arguments = ast_parse_call_function_arguments(arena, parser);

  Expression *expression = arena_alloc(arena, sizeof(Expression));

  expression->type = FUNCTION_CALL_EXP;
  expression->function_call = function_call;

  return expression;
}

Expression **ast_parse_call_function_arguments(Arena *arena, Parser *parser) {

  Expression **arguments = arena_array_with_cap(arena, Expression *, 16);
  Expression *tmp = NULL;

  ast_next_token(arena, parser);
  tmp = ast_parse_expression(arena, parser, LOWEST_PREC);
  // TODO after REFACTOR make this a ** cause this is kinda uwu owo lazyyyy
  // but maybe making this rn could make the initial refactor even worse!
  // making a memory bongus here btw
  append(arguments, tmp);

  while (peek_token_is(parser, COMMA)) {
    ast_next_token(arena, parser);
    ast_next_token(arena, parser);

    tmp = ast_parse_expression(arena, parser, LOWEST_PREC);
    append(arguments, tmp);
  }

  ast_expect_peek_token(arena, parser, R_PAREN);

  return arguments;
}

Expression **ast_parse_array_members(Arena *arena, Parser *parser) {

  Expression **arguments = arena_array_with_cap(arena, Expression *, 16);
  Expression *tmp = NULL;

  ast_next_token(arena, parser);
  tmp = ast_parse_expression(arena, parser, LOWEST_PREC);
  // TODO after REFACTOR make this a ** cause this is kinda uwu owo lazyyyy
  // but maybe making this rn could make the initial refactor even worse!
  // making a memory bongus here btw
  append(arguments, tmp);

  while (peek_token_is(parser, COMMA)) {
    ast_next_token(arena, parser);
    ast_next_token(arena, parser);

    tmp = ast_parse_expression(arena, parser, LOWEST_PREC);
    append(arguments, tmp);
  }

  ast_expect_peek_token(arena, parser, R_SQUARE_BRACE);

  return arguments;
}

Expression *ast_parse_infix_expression(Arena *arena, Parser *parser,
                                       Expression *left) {
  InfixExpression infix_exp = (InfixExpression){0};
  infix_exp.token = parser->curr_token;
  infix_exp.operator= parser->curr_token.literal;
  infix_exp.left = left;

  Precedence precedence =
      get_precedence_from_hm(PRECENDENCES, parser->curr_token.type);
  ast_next_token(arena, parser);
  infix_exp.right = ast_parse_expression(arena, parser, precedence);

  Expression *expression = arena_alloc(arena, sizeof(Expression));
  expression->type = INFIX_EXP;
  expression->infix = infix_exp;

  return expression;
}

Expression *ast_parse_prefix_expression(Arena *arena, Parser *parser) {
  PrefixExpression prefix_exp = (PrefixExpression){0};
  prefix_exp.token = parser->curr_token;
  prefix_exp.operator= parser->curr_token.literal;
  ast_next_token(arena, parser);
  prefix_exp.right = ast_parse_expression(arena, parser, PREFIX_PREC);

  Expression *expression = arena_alloc(arena, sizeof(Expression));
  expression->type = PREFIX_EXP;
  expression->prefix = prefix_exp;

  return expression;
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
Expression *ast_parse_expression(Arena *arena, Parser *parser,
                                 Precedence precedence) {
  prefix_parse_fn prefix =
      get_prefix_fn_from_hm(PARSING_FUNCTIONS, parser->curr_token.type);
  if (!prefix) {
    String error = arena_string_fmt(
        arena, PARSING_ERROR "Not prefix function found for: %s",
        parser->curr_token.literal.str,
        get_token_literal(parser->curr_token.type));
    ast_parser_error_append(parser, error);
    return NULL;
  }

  Expression *left_value = prefix(arena, parser);

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

  ExpressionStatement expression_statement = (ExpressionStatement){0};
  expression_statement.token = initial_token;

  expression_statement.expression_value =
      ast_parse_expression(arena, parser, LOWEST_PREC);

  statement->token = initial_token;
  statement->type = EXPRESSION_STATEMENT;
  statement->expression_statement = expression_statement;

  if (peek_token_is(parser, SEMICOLON)) {
    ast_next_token(arena, parser);
  }

  return statement;
}

// make other stuff return null if something fails i thnk
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

Node *ast_parse_statements(Arena *arena, Parser *parser, TokenType delimiter) {
  Node *statements = arena_array(arena, Node);
  while (parser->curr_token.type != delimiter &&
         parser->curr_token.type != EOF_) {
    Node *stmnt = ast_parse_statement(arena, parser);
    if (stmnt) {
      append(statements, *stmnt);
    }
    ast_next_token(arena, parser);
  }
  return statements;
}

// reads before anything
BlockStatement ast_parse_block_statement(Arena *arena, Parser *parser) {
  Node *statements = arena_array(arena, Node);
  ast_next_token(arena, parser);
  while (parser->curr_token.type != R_BRACE &&
         parser->curr_token.type != EOF_) {
    Node *stmnt = ast_parse_statement(arena, parser);
    if (stmnt) {
      append(statements, *stmnt);
    }
    ast_next_token(arena, parser);
  }
  return (BlockStatement){.statements = statements};
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
    print_statement(arena, &program->statements[i]);
  }
}

void print_parser_errors(Parser parser) {
  if (len(parser.errors)) {
    printf("Tal vez no funciona en burros\n");
    for (size_t i = 0; i < array_len(BURRO); i++) {
      printf("%s\n", BURRO[i]);
    }
  } else {
    printf("There are not parsing errors\n");
  }
  for (size_t i = 0; i < len(parser.errors); i++) {
    Error err = parser.errors[i];
    printfln("%S\n", err.error);
  }
}

String stringify_statements(Arena *arena, Node *statements) {
  String result = arena_new_empty_string_with_cap(arena, 256);
  for (size_t i = 0; i < len(statements); i++) {
    arena_string_concat(arena, &result,
                        arena_stringify_statement(arena, &statements[i]));
    arena_string_concat(arena, &result, string(";"));
  }
  return result;
}

// todo append also a NO DO NOT APPEND A NEW LINE BRO
String stringify_program(Arena *arena, Program *program) {
  String result = arena_new_empty_string_with_cap(arena, 256);
  /*String new_line = string("\n\t");*/
  for (size_t i = 0; i < len(program->statements); i++) {
    arena_string_concat(
        arena, &result,
        arena_stringify_statement(arena, &program->statements[i]));
  }
  return result;
}

void print_expression() {
  // TOOD PLEASE SDOMETHING THAT WORKS INSIDE THE DEBUGGER
}

// TODO: remove node from args
//       remove .str from Strings when formatting, i can use the %S format
//       specifier to print them, forgot i implemented it XD
String stringify_expression(Arena *arena, Node *node, Expression *expression) {
  (void)node;
  String exp_string = {0};
  if (!expression) {
    return string("(null)");
  }
  switch (expression->type) {
  case IDENTIFIER_EXP: {
    Identifier parsed_identifier = expression->identifier;
    exp_string =
        arena_string_fmt(arena, "%s", parsed_identifier.token.literal.str);
    break;
  }
  case INTEGER_LIT_EXP: {
    IntLiteral parsed_int = expression->integer_literal;
    // shit explodes here huhu it overwrites the bang memory for some reason
    // good luck!
    exp_string = arena_string_fmt(arena, "%d", parsed_int);
    /*exp_string = string("5");*/
    break;
  }
  // here we should use a tmp arena for strings, after finishing the
  // for loop we could just free everything and should be fineee and cool
  // and sexy
  case ARRAY_EXP: {
    Array array = expression->array;
    Arena tmp_arena = {0};
    String member_str = {0};

    String tmp_members_string =
        arena_new_empty_string_with_cap(&tmp_arena, 128);
    arena_string_concat(&tmp_arena, &tmp_members_string, string("["));

    for (U64 i = 0; i < len(array.value); i++) {
      if (i > 0) {
        arena_string_concat(&tmp_arena, &tmp_members_string, string(", "));
      }
      Expression *exp = expression->array.value[i];
      member_str = stringify_expression(&tmp_arena, node, exp);
      arena_string_concat(&tmp_arena, &tmp_members_string, member_str);
    }

    arena_string_concat(arena, &tmp_members_string, string("]"));
    exp_string = arena_string_fmt(arena, "%S", tmp_members_string);
    arena_free(&tmp_arena);
    break;
  }
  case BOOLEAN_EXP: {
    Boolean parsed_bool = expression->boolean;
    exp_string = arena_string_fmt(arena, "%b", parsed_bool);
    break;
  }
  case PREFIX_EXP: {
    PrefixExpression prefix = expression->prefix;
    String operator= prefix.operator;
    String right = stringify_expression(arena, node, prefix.right);
    exp_string = arena_string_fmt(arena, "(%s%s)", operator.str, right.str);
    break;
  }
  case IF_EXP: {
    IfExpression if_expression = expression->if_expression;
    String condition, consequence, alternative;

    condition = stringify_expression(arena, node, if_expression.condition);

    consequence =
        stringify_statements(arena, if_expression.consequence.statements);

    if (if_expression.alternative.statements) {
      alternative =
          stringify_statements(arena, if_expression.alternative.statements);
      exp_string = arena_string_fmt(arena, "if (%s) {%s} else {%s}",
                                    condition.str,   //
                                    consequence.str, //
                                    alternative.str);
    } else {
      exp_string = arena_string_fmt(arena, "if (%s) {%s}",
                                    condition.str, //
                                    consequence.str);
    }

    break;
  }
  case FUNCTION_LITERAL_EXP: {
    FunctionLiteral function = expression->function_literal;
    String content, arguments_names_str, token, name;
    Identifier *arguments_names;

    arguments_names_str = arena_new_empty_string_with_cap(arena, 128);
    token = function.token.literal;
    name = function.name.value;
    arguments_names = function.parameters;

    // TODO: put this in another function if still feels annoying to see
    // this is also something useful to put in a function cause function calls
    // also have arguments to stringify so yeah
    for (I64 i = 0; i < len(arguments_names); i++) {
      if (i > 0) {
        // TODO: remove trailing space just put it here because is pretty
        // for now
        arena_string_concat(arena, &arguments_names_str, string(", "));
      }
      String arg = arguments_names[i].value;
      string_concat(&arguments_names_str, arg);
    }

    if (function.body.statements) {
      content = stringify_statements(arena, function.body.statements);
      exp_string = arena_string_fmt(arena, "%s %s(%s) {%s}",
                                    token.str,               //
                                    name.str,                //
                                    arguments_names_str.str, //
                                    content.str);
    } else {
      exp_string = arena_string_fmt(arena, "%s (%s)",
                                    token.str, //
                                    name.str,  //
                                    content.str);
    }

    break;
  }
  case FUNCTION_CALL_EXP: {
    FunctionCallExpression function = expression->function_call;
    String function_name, joined_args;
    Expression **arguments;

    function_name = stringify_expression(arena, node, function.function);
    arguments = function.arguments;

    joined_args = arena_join_expression_array(arena, arguments, string(", "));
    exp_string =
        arena_string_fmt(arena, "%s(%s)", function_name.str, joined_args.str);
    break;
  }
  case INFIX_EXP: {
    InfixExpression infix = expression->infix;
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
        "There has not been a EXPRESSION STRING for this expression type: %d\n",
        expression->type);
  }
  }
  return exp_string;
}

// todo
String arena_stringify_statement(Arena *arena, Node *node) {
  String str_stmt;
  switch (node->type) {
  case LET_STATEMENT: {
    LetStatement statement = node->let_statement;
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
      ReturnStatement statement = node->return_statement;
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
      ExpressionStatement statement = node->expression_statement;
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

void print_statement(Arena *arena, Node *node) {
  String statement = arena_stringify_statement(arena, node);
  printf("%s\n", statement.str);
}

/*void print_expression(Expression expression) {*/
/*  switch (expression.type) {*/
/*    case :*/
/*  }*/
/*}*/

#endif /* ifndef _PARSER_H */
