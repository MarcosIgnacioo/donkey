#ifndef _PARSER_H
#include "../arena.c"
#include "../arena_strings.c"
#include "../array.c"
#include "../lexer.c"
#include "../token.c"
#define color(C) "\033[0;3" #C "m"
#define end_color "\033[0m"
#define SYNTAX_ERROR color(1) "[SYNTAX ERROR] : " end_color

typedef enum {
  NODE_NIL,
  STATEMENT,
  EXPRESSION,
  NIL_STATEMENT,
  LET_STATEMENT,
  IF_STATEMENT,
  FUNCTION_STATEMENT,
  IDENTIFIER_STATEMENT,
} NodeType;

typedef struct {
  Token token;
} Expression;

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

// fn defs
void ast_next_token(Arena *arena, Parser *parser);

Parser ast_new_parser(Arena *arena, Lexer *lexer);

Program ast_parse_program(Arena *arena, Parser *parser);
Node *ast_parse_statement(Arena *arena, Parser *parser);
Node *ast_parse_let_statement(Arena *arena, Parser *parser);
bool ast_expect_peek_token(Arena *arena, Parser *parser,
                           TokenType expected_type);
void ast_parser_peek_error(Arena *arena, Parser *parser,
                           TokenType expected_type);

#define ast_token_literal(NODE) (((Token *)NODE)->literal)

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

  while (parser->curr_token.type != SEMICOLON) {
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
  default:
    //
    {
      return ast_parse_dummy_statement(arena, parser);
    }
  }
}

String ast_init(Program *program) {
  if (len(program->statements)) {
    return ast_token_literal(&program->statements[0]);
  } else {
    return string("");
  }
}

#endif /* ifndef _PARSER_H */
