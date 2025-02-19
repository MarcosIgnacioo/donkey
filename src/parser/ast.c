#ifndef _PARSER_H
#include "../arena.c"
#include "../arena_strings.c"
#include "../array.c"
#include "../lexer.c"
#include "../token.c"
#define color(C) "\033[0;3" #C "m"
#define end_color "\033[0m"
#define SYNTAX_ERROR color(3) "[SYNTAX ERROR] : " end_color
typedef long long I64;

typedef enum {
  NIL_STATEMENT,
  LET_STATEMENT,
  RETURN_STATEMENT,
  EXPRESSION_STATEMENT,
  // expressions
  IDENTIFIER_EXP,
  INTEGER_LIT_EXP,
} NodeType;

typedef enum {
  NIL_PREC,
  LOWEST_PREC,
  EQUALS_PREC,      // ==
  LESSGREATER_PREC, // > or <
  SUM_PREC,         // +
  PRODUCT_PREC,     // *
  PREFIX_PREC,      // -X or !X
  CALL_PREC,        // myFunction(X)
} Precedence;

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
  Identifier name; // removed *
  Expression value;
} LetStatement;

typedef struct {
  Token token;
  Expression value;
} ReturnStatement;

typedef struct {
  Token token; // first token of the expression
  Expression value;
} ExpressionStatement;

typedef struct {
  Token token;
  NodeType type;
  void *data;
} Node;

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
typedef Expression (*infix_parse_fn)(Arena *, Parser *, Node *, Node *);

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
Expression ast_parse_int(Arena *arena, Parser *parser);
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

#define prs_fn(INFIX, PREFIX)                                                  \
  (ParsingFunctions) { .prefix_fn = INFIX, .infix_fn = PREFIX }
KeyValue_PF FUNCTIONS_ARR[] = {
    kv(KeyValue_PF, ILLEGAL, prs_fn(NULL, NULL)), // parsing stuff
    kv(KeyValue_PF, EOF_, prs_fn(NULL, NULL)),    //
    kv(KeyValue_PF, IDENTIFIER, prs_fn(&ast_parse_identifier, NULL)), //
    kv(KeyValue_PF, INT, prs_fn(&ast_parse_int, NULL)),               //
    kv(KeyValue_PF, ASSIGN, prs_fn(NULL, NULL)),                      //
    kv(KeyValue_PF, MINUS, prs_fn(NULL, NULL)),                       //
    kv(KeyValue_PF, PLUS, prs_fn(NULL, NULL)),                        //
    kv(KeyValue_PF, ASTERISK, prs_fn(NULL, NULL)),                    //
    kv(KeyValue_PF, SLASH, prs_fn(NULL, NULL)),                       //
    kv(KeyValue_PF, EQUALS, prs_fn(NULL, NULL)),                      //
    kv(KeyValue_PF, BANG, prs_fn(NULL, NULL)),                        //
    kv(KeyValue_PF, NOT_EQUALS, prs_fn(NULL, NULL)),                  //
    kv(KeyValue_PF, LT, prs_fn(NULL, NULL)),                          //
    kv(KeyValue_PF, GT, prs_fn(NULL, NULL)),                          //
    kv(KeyValue_PF, G_EQUALS, prs_fn(NULL, NULL)),                    //
    kv(KeyValue_PF, L_EQUALS, prs_fn(NULL, NULL)),                    //
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
    kv(KeyValue_PF, TRUE, prs_fn(NULL, NULL)),      //
    kv(KeyValue_PF, FALSE, prs_fn(NULL, NULL)),     //
    kv(KeyValue_PF, RETURN, prs_fn(NULL, NULL)),    //
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

prefix_parse_fn get_prefix_fn_from_hm(HashTable table, TokenType key) {
  KeyValue_PF *kv = hash_table_find_item(PARSING_FUNCTIONS, &key);
  if (kv) {
    return kv->value.prefix_fn;
  } else {
    return NULL;
  }
}

infix_parse_fn get_infix_fn_from_hm(HashTable table, TokenType key) {
  KeyValue_PF *kv = hash_table_find_item(PARSING_FUNCTIONS, &key);
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

#define peek_token_is(P, E) P->peek_token.type == E
#define curr_token_is(P, E) P->curr_token.type == E

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
  let_statement->value = ast_parse_expression(arena, parser, LOWEST_PREC);
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
  return (Expression){.type = IDENTIFIER_EXP,
                      .exp_bytes =
                          (void *)arena_token_clone(arena, parser->curr_token)};
}

// TODO:
// HOW do we recover from error here
// gamer idea just return a pointer dude, the problem
// is that would require to allocate in the heap and
// we are divorced, an empty struct will do the trick
// maybe a expression nil or something like that
// like the one randys did in the game dev videos
Expression ast_parse_expression(Arena *arena, Parser *parser,
                                Precedence prece) {
  prefix_parse_fn prefix =
      get_prefix_fn_from_hm(PARSING_FUNCTIONS, parser->curr_token.type);
  if (!prefix) {
    return (Expression){0};
  }
  Expression left_value = prefix(arena, parser);
  return left_value;
}

Node *ast_parse_expression_statement(Arena *arena, Parser *parser) {
  Node *statement = arena_alloc(arena, sizeof(Node));
  ExpressionStatement *expr_statement =
      arena_alloc(arena, sizeof(ExpressionStatement));
  expr_statement->value = ast_parse_expression(arena, parser, LOWEST_PREC);
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
  for (size_t i = 0; i < len(program->statements); i++) {
    print_statement(arena, program->statements[i]);
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
    exp_string = arena_string_fmt(arena, "%s", parsed_identifier.token.literal);
    break;
  }
  case INTEGER_LIT_EXP: {
    IntLiteral parsed_int = *(IntLiteral *)expression.exp_bytes;
    exp_string = arena_string_fmt(arena, "%d", parsed_int);
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
  case LET_STATEMENT:
    // i put ; in this but what if the user didnt put them huhhhh
    {
      LetStatement statement = *((LetStatement *)node.data);
      String expression_string =
          stringify_expression(arena, node, statement.value);
      str_stmt = arena_string_fmt(     //
          arena,                       //
          "%s %s = %s;",               //
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
          stringify_expression(arena, node, statement.value);
      str_stmt = arena_string_fmt(arena, "%s %s;",             //
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
          stringify_expression(arena, node, statement.value);
      str_stmt = arena_string_fmt(arena, "%s;",         //
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

#endif /* ifndef _PARSER_H */
