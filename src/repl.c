#ifndef _REPL_H
#define _REPL_H
#include "./object/object.h"
#include "./parser/ast.c"
#include "arena.c"
#include "arena_strings.c"
#include "lexer.c"
#include <stdio.h>
#define MAX_LINE_CHARS 512
#define color(C) "\033[0;3" #C "m"
#define end_color "\033[0m"

// how does ctrl d work because it sends something but fgets doesnt get it null
// terminated which is weird, so i need to memory set it to zero always i could
// just nulll char the last element of the arra but if there is trash data well
// it will print out all the trash data in the array so i just zero everything
// out
String get_line_stdin(Arena *arena) {
  char line_buff[MAX_LINE_CHARS];
  memory_set(line_buff, MAX_LINE_CHARS * sizeof(char), 0);
  fgets(line_buff, MAX_LINE_CHARS, stdin);
  return arena_new_string(arena, line_buff);
}

void donkey_repl(Arena *arena) {
  String input = {0};
  Lexer lexi = (Lexer){0};
  Parser parser = {0};
  Program program = {0};
  String program_str;
  Object evaluation;
  do {
    input = get_line_stdin(arena);
    lexi = lexer_new_lexer(input);
    parser = ast_new_parser(arena, &lexi);
    program = ast_parse_program(arena, &parser);
    evaluation = eval_evaluate_program(arena, program);
    program_str = object_to_string(arena, evaluation);
    if (len(parser.errors)) {
      print_parser_errors(parser);
    } else if (program_str.len) {
      printfln(color(4) "DONKEY >> " end_color "%S", program_str);
    }
    /*arena_reset(arena);*/
  } while (input.len);
}

#endif /* ifndef _REPL_H */
