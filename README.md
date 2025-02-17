# TODO

make the functions inprefix and prefix hashmap to be a macro like making the macro that takes the name of the enum variable
and then it makes also an array of strings with that name (this is achieved with xmacros)
so i can access the array within just the enum value 




Something really cool is the way we manage errors in the parser

We have just an array of errors (which is just a type for a string, we do this, so if in a future we wanna put more stuff to our errors, it is as easy as modifying just the struct)

And then we have a function that does the error stuff, which is pretty simple

It just creates a string with the error, message, this kind of error are the ones when we dont have the token we expect, we accept the expected TokenType, and squash the one in the *peek_token* string; BECAUSE WE ACTIVATE THIS ERROR ONLY WHEN WE ARE PEEKING
```c
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

```
which is done by this function, this is used to check if the peeking token is the same type as the expected one
if is not, it calls the ast_parser_peek_error fn to register the error and vuala
```c
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
```

this kind of data piping is what i wanna achieve is so freaking cool






```c
Program ast_init_program(Arena *arena, Token *tokens) {
  Statement *statements = arena_array(arena, Statement);
  Token *statement_tokens = arena_array(arena, Token);
  for (U64 i = 0; i < len(tokens); i++) {
    Token tok = tokens[i];
    if (tok.type != SEMICOLON) {
      append(statement_tokens, tok);
      continue;
    }
    Token first_tok = statement_tokens[0];
    NodeType node_type = NODE_NIL;
    void *data = NULL;
    switch (first_tok.type) {
    case LET:
      //
      {
        data = arena_alloc(arena, sizeof(LetStatement));
        node_type = LET_STATEMENT;
          for (i = 0; i < len(statement_tokens); i++) {
            if (statement_tokens[i].type == IDENTIFIER) {
              (LetStatement *)data->identifier = statement_tokens[i].tok
            }
          }
      }
    case IF:
      //
      {
        node_type = IF_STATEMENT;
      }
    case FUNCTION:
      //
      {
        node_type = FUNCTION_STATEMENT;
      }
    default:
      //
      {
        node_type = IDENTIFIER_STATEMENT;
      }
    }
    append(statements, (Node){.type = node_type, .token=first_tok, .data = data})
  }
  return (Program){.statements = statements};
}
  Foo f = (Foo){.name = "NOMBRE", .z = 512};
  Fuzz fz = (Fuzz){.tok = f,
                   .y = {1, 2, 3, 4, 5, 6,  7,  8, 9,   0,   1,  2, 3,  4, 5,
                         6, 7, 8, 9, 1, 56, 23, 3, 123, 123, 45, 4, 12, 45}};
  Bar b = (Bar){.tok = f, .x = 327};
  void *ptr = &fz;
  void *ptr2 = &b;
  printf("%d\n", (((Fuzz *)(((Foo *)ptr) + 12))->y)[1]);
  printf("%s\n", ((Foo *)ptr2)->name);
```

