#ifndef _REPL_H
#define _REPL_H
#include "arena.c"
#include "arena_strings.c"
#include "lexer.c"
#include <stdio.h>
#define MAX_LINE_CHARS 3
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
  String input = (String){0};
  Token tok = (Token){0};
  Lexer lexi = (Lexer){0};
  do {
    input = get_line_stdin(arena);
    lexi = lexer_new_lexer(input);
    tok = lexer_next_token(arena, &lexi);
    while (tok.type != EOF_) {
      print_token(color(4) "DONKEY >>" end_color, tok);
      tok = lexer_next_token(arena, &lexi);
    }
  } while (input.len);
}

#endif /* ifndef _REPL_H */
