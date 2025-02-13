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

