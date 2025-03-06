```c 
typedef struct {
  char *input;
  XXXXX expected;
} TestResultXXXXX;

void test_XXXXX_evaluations() {
  TestResultXXXXX test_cases[] = {
      (TestResultXXXXX){.input = "", .expected = },
      (TestResultXXXXX){.input = "", .expected = },
  };
  Object test_obj;
  XXXXX pass = true;
  for (I64 i = 0; i < array_len(test_cases); i++) {
    TestResultXXXXX test = test_cases[i];
    test_obj = test_eval(test.input);
    if (!test_object_XXXXX(test_obj, test.expected)) {
      pass = false;
    }
  }
  printfln("Last expression evaluated to: %S", object_to_string(&arena, test_obj));

  // TODO: Find a way to make the part of the function name not be hardcoded and
  //       just in a macro cause its better!! i hope
  if (pass) {
    printf(LOG_SUCCESS "ALL TEST PASSED AT: test_XXXXX_evaluations() \n");
  } else {
    printf(LOG_ERROR "TEST FAILED       AT: test_XXXXX_evaluations() \n");
  }
}
```
TODO monday tuesday

[x] which may imply to refactor a lot of stuff to 
    unions instead of void *
[ ] start second refactor where i change the expressions array in the function calls to be an Expressions ** so i dont need to copy data and just pass the pointers of the already existing expressions
[ ] start hallucinating the testing api i wanna have so testing aint as tedius
[ ] start remaking classmoods

todo: eventually
why the output of the program when parsing numbers is different when compiled with clang and gcc 
```sh
input : 1 + 9 + 123
clang : DONKEY >>return ((1 + 9) + 123) yay
gcc   : DONKEY >>return ((1 + 1) + 3)   not much yay
```
```c

  ast_next_token();
  while (!ast_expect_curr_token(parser, R_PAREN)) {
    ident = ast_parse_identifier(arena, parser, LOWEST_PREC);
    append()
    ast_next_token(arena, parser); // reads to the next token,
  }

  why this NOT?
    cause fn foo()

  because here the curr_token would be (
  and when we entered the loop we would advance to
  the next one ) and try to parse it as an identifier
  which well is not an identifier!!!!

  and in the next one this doenst happen
  while (!ast_expect_curr_token(parser, R_PAREN)) {
    ast_next_token();
    ident = ast_parse_identifier(arena, parser, LOWEST_PREC);
    append()
  }
bool generic_test_literal_expression(Expression exp, va_list args) {
  switch (exp.type) {
  case IDENTIFIER_EXP:
    //
    {
      String expected = va_arg(args, String);

      return test_identifier(exp, expected);
      break;
    }
  case INTEGER_LIT_EXP:
    //
    {
      I64 expected = va_arg(args, I64);

      return test_integer_literal(exp, expected);
      break;
    }
  case BOOLEAN_EXP:
    //
    {
      bool expected = va_arg(args, bool);

      return test_bool_literal(exp, expected);
      break;
    }
  default:
    //
    {
      print_error("There is no helper function to this expression type\nIm so "
                  "sorry for that\n");
      return false;
      break;
    }
  }
}
int test_foo() {
  Arena arena = (Arena){.begin = NULL, .end = NULL};

  typedef struct {
    union {
      String token_literal;
      bool value;
    } data;
  } Expected;

  typedef struct {
    String input;
    Expected expected;
  } Test;

  Test expected_identifiers[] = {
      //
      {
          .input = string("let x = true;"),
          .expected =
              (Expected){
                  .token_literal = string("true"),
                  .value = true,
              },
      },
      //
      {
          .input = string("let x = true;"),
          .expected =.args = {"x", true},
          (Expected){
              .token_literal = string("true"),
              .value = true,
          },
      },
      //
  };

  for (int i = 0; i < array_len(expected_identifiers); i++) {
    failed = 0;
    Test test = expected_identifiers[i];
    String input = test.input;
    Lexer lexer = lexer_new_lexer(input);
    Parser parser = ast_new_parser(&arena, &lexer);
    Program program = ast_parse_program(&arena, &parser);
    print_parser_errors(parser);
    failed = !test_statement(program.statements[0], &test.expected.value);
    /*failed = !test_literal_expression(exp_stmnt.expression_value,*/
    /*                                  &test.expected.value);*/
    if (!failed) {
      printf(LOG_SUCCESS "There are not parsing errors!\n");
    }
  }

  arena_free(&arena);
  return failed;
}
```
fix this token illegal that i forgot to change when parsing a boolean it is not that important tho
```c
p *(ExpressionStatement *)statement.data
$7 = {token = {type = ILLEGAL, literal = {str = 0x0, len = 0, cap = 0}}, 
  expression_value = {type = BOOLEAN_EXP, exp_bytes = 0x55555556e358}}
```

maybe making all the test use __va_args__ makes the making of tests less boring and shitty and poop

creo que la manera de lograr conseguir interfaces en c es la siguiente

```c
typedef struct {
  ExpressionType type;
  union {
    LetStatement let_statement;
    ReturnStatement return_statement;
    ExpressionStatement expression_statement;
  } data;
} Expression;
```

en codigo puedo acceder a los campos con un switch case

dependiendo del tipo que sea pues se hacen las operaciones con cada tipo de Expression

```c
p *(IntLiteral *)(((InfixExpression *)resulting_exp.exp_bytes)->left)->exp_bytes
p *(Identifier *)(((expres *)resulting_exp.exp_bytes)->left)->exp_bytes
p *(IntLiteral *)(((InfixExpression *)resulting_exp.exp_bytes)->right)->exp_bytes
p (((InfixExpression *)resulting_exp.exp_bytes)->operator) 
```

a + b * c

lef

```c
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
    left_value => .exp_bytes*Identifier("a");
    precedence => LOWEST_PRECEDENCE;

  while (!peek_token_is(parser, SEMICOLON) &&
         precedence < peek_precedence(parser)) {

    {
      i = 1 {
        peek_precedence(parser) => SUM_PRECEDENCE;
      },
    }

    infix_parse_fn infix =
        get_infix_fn_from_hm(PARSING_FUNCTIONS, parser->peek_token.type);

    if (!infix) {
      return left_value;
    }

    ast_next_token(arena, parser);
    left_value = infix(arena, parser, left_value);
  }

  return left_value;
```


ctrl shift 1 changes to that tab in that buffer idk
# doc for cool stuff


La razon por la que lo hacemos con el peek_token en evez de avanzar el token directamente
   y usar el current_token es porque, el peeking token podria no tener ninguna funcion de 
   infix por lo que pues se entra al if, y hemos leido un token que no nos correspondia
   por ejemplo este:

```c
let x = 1 + 2 [ 3;
```
   este el [ no tiene una funcion de parseo, pero como en ese aproach se avanzo
   con el parser para checar esta parte, pues no es hasta en este punto que sabemos
   que no se puede parsear como parte de una expresion,
   por lo que retornamos, pero habiendo consumido ese token, por eso, siempre
   intentamos hacerlo mejor con el peek token
   puesto que no tiene este problema y es literalmente el mismo codigo

# OLD

```c
ast_parse_expression() fn 466
...
  while (!peek_token_is(parser, SEMICOLON) && precedence < peek_precedence(parser)) {
    ast_next_token(arena, parser); // <-

    infix_parse_fn infix =
        get_infix_fn_from_hm(PARSING_FUNCTIONS, parser->current_token.type);

    if (!infix) {
      return left_value;
    }

    left_value = infix(arena, parser, left_value);
  }
```
                                            VS
# BOOK
```c
  while (!peek_token_is(parser, SEMICOLON) && precedence < peek_precedence(parser)) {
    infix_parse_fn infix =
        get_infix_fn_from_hm(PARSING_FUNCTIONS, parser->peek_token.type);

    if (!infix) {
      return left_value;
    }

    ast_next_token(arena, parser); // <-
    left_value = infix(arena, parser, left_value);
  }
```
# TODO

```
p (((((Array_Header *)program.statements) - 1))->length)
```

[x]make the functions inprefix and prefix hashmap to be a macro like making the macro that takes the name of the enum variable
and then it makes also an array of strings with that name (this is achieved with xmacros)
so i can access the array within just the enum value 

[] refactor 

    statement -> let_statement || return_statement || expression_statement
      then the statement has different things for each one of its kind
    a let statement has a token (the let token), a Identifier, which is a token with the name of the thing we are declaring and a Expression with the value we wanna store in that variable
    the problem is it is kinda hard to navigate through all this structs and remembering the stuff each one has, and how we wanna operate this thing





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

