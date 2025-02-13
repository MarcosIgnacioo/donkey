#ifndef _PARSER_H
#include "../arena.c"
#include "../arena_strings.c"
#include "../array.c"
#include "../lexer.c"
#include "../token.c"

typedef enum {
  NODE_NIL,
  STATEMENT,
  EXPRESSION,
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
  NodeType type;
  void *data;
} Node;

typedef struct {
  Node *statements;
} Program;

typedef struct {
  Lexer *lexer;
  Token curr_token;
  Token peek_token;
} Parser;

// fn defs
void ast_next_token(Arena *arena, Parser *parser);

Parser ast_new_parser(Arena *arena, Lexer *lexer);

Program ast_parse_program(Arena *arena, Parser *parser);
Node *ast_parse_statement(Arena *arena, Parser *parser);
Node *ast_parse_let_statement(Arena *arena, Parser *parser);

#define ast_token_literal(NODE) (((Token *)NODE)->literal)

Parser ast_new_parser(Arena *arena, Lexer *lexer) {
  Parser parser = (Parser){
      .lexer = lexer, .curr_token = (Token){0}, .peek_token = (Token){0}};
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

bool ast_expect_peek_token(Parser *parser, TokenType expected_type) {
  return parser->peek_token.type == expected_type;
}

Node *ast_parse_let_statement(Arena *arena, Parser *parser) {
  Node *statement = arena_alloc(arena, sizeof(Node));
  LetStatement *let_statement = arena_alloc(arena, sizeof(LetStatement));
  statement->type = LET_STATEMENT;
  statement->data = let_statement;
  let_statement->token = parser->curr_token;
  if (peek_token_is(parser, IDENTIFIER)) {
    ast_next_token(arena, parser);
    let_statement->name = (Identifier){.token = parser->curr_token,
                                       .value = parser->curr_token.literal};
  }

  if (!(peek_token_is(parser, ASSIGN))) {
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
      return NULL;
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
