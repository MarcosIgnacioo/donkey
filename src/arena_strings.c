#ifndef _ARENA_STRINGS_H
#include "array.c"
#include "arena.c"
#include <stdbool.h>
#define _ARENA_STRINGS_H
typedef unsigned long U64;
typedef long long I64;
typedef char byte;

#define string(STRING)                                                         \
  (String) { .str = STRING, .len = str_len(STRING), .cap = str_len(STRING) }
#define slice(STRING, BEGIN, END)                                              \
  (String) { .str = STRING.str + BEGIN, .len = END - BEGIN, .cap = END - BEGIN }
#define c_slice(STRING, BEGIN, END)                                              \
  (String) { .str = STRING + BEGIN, .len = END - BEGIN, .cap = END - BEGIN }

typedef struct {
  byte *str;
  U64 len;
  U64 cap;
} String;

typedef bool (*TrimmerPredicator)(char);
typedef bool (*Predicator)(String, String);

// converting a c string into an arena string and returning the string created
// in the stack! well is in the arena the pointer but but the struct that
// contains the pointers is in the stack so shutup
U64 str_len(const char *str);
String arena_new_string(Arena *arena, const char *source);
String arena_new_string_with_len(Arena *arena, const char *source, U64 len);
String arena_new_empty_string_with_cap(Arena *arena, U64 capacity);
String arena_new_string_zero(Arena *arena, const char *source);
String arena_string_slice(Arena *arena, String chopping, U64 start, U64 end);
String arena_string_append(Arena *arena, String body, String appending);
String *arena_string_split(Arena *arena, String chopping, String delimiter);
String arena_string_realloc(Arena *arena, String source);
void arena_string_ptr_realloc_with_cap(Arena *arena, String *source,
                                       U64 new_cap);

// files
String arena_read_file(Arena *arena, const char *file_name);
void read_file(String *file_content, const char *file_name);

// maybe
void string_trim_space_left(String *trimming);
void string_trim_space_right(String *trimming);
void string_trim_space(String *trimming);
void string_concat(String *dest, String source);
void arena_c_string_concat(Arena *arena, String *dest, const char *source);

void string_trim_predicator_right(String *trimming,
                                  TrimmerPredicator predicator);
void string_trim_predicator_left(String *trimming,
                                 TrimmerPredicator predicator);
void string_trim_predicator(String *trimming, TrimmerPredicator predicator);
void c_string_copy(String *to, const char *from);
void string_copy(String *to, String from);

bool is_lower_case(char c) { return c >= 'a' && c <= 'z'; }
bool memory_cmp(byte *this, byte *that, U64 length);
bool string_equals(String this, String that);

int printfln(const char *str, ...);

// deprecated
void print_string(String str);
void print_stringln(String str);

// IMPLEMENTATION
U64 str_len(const char *str) {
  int len = 0;
  while (*str++) {
    len++;
  }
  return len;
}

String arena_new_string_zero(Arena *arena, const char *source) {
  U64 string_len = str_len(source);
  String new_string =
      (String){.str = NULL, .len = string_len, .cap = string_len};
  byte *arena_str = arena_alloc_zero(arena, string_len);
  memory_copy(arena_str, source, string_len);
  new_string.str = arena_str;
  return new_string;
}



String arena_new_string_with_len(Arena *arena, const char *source, U64 len) {
  String new_string =
      (String){.str = NULL, .len = len, .cap = len};
  byte *arena_str = arena_alloc_zero(arena, len);
  memory_copy(arena_str, source, len);
  new_string.str = arena_str;
  return new_string;
}

// tofix some elements dont initalize with cap or they just have zero
String arena_new_string(Arena *arena, const char *source) {
  U64 string_len = str_len(source);
  String new_string =
      (String){.str = NULL, .len = string_len, .cap = string_len};
  byte *arena_str = arena_alloc_zero(arena, string_len);
  memory_copy(arena_str, source, string_len);
  new_string.str = arena_str;
  return new_string;
}

String arena_new_empty_string_with_cap(Arena *arena, U64 capacity) {
  String new_string = (String){.str = NULL, .len = 0, .cap = capacity};
  byte *arena_str = arena_alloc_zero(arena, capacity);
  new_string.str = arena_str;
  return new_string;
}

int min(int a, int b) { return (a < b) ? a : b; }

String arena_string_slice(Arena *arena, String chopping, U64 start, U64 end) {
  end = min(end, chopping.len);
  start = min(start, 0);
  return (String){.str = chopping.str + start, .len = end - start};
}

String arena_string_append(Arena *arena, String body, String appending) {
  U64 total_size = body.len + appending.len;
  byte *new_str = arena_alloc_zero(arena, total_size);
  memory_copy(new_str, body.str, body.len);
  memory_copy(new_str + body.len, appending.str, appending.len);
  return (String){.str = new_str, .len = total_size};
}

// superslow cmp but the delimiter can be a whole string!
String *arena_string_split(Arena *arena, String chopping, String delimiter) {
  String *tokens = arena_array(arena, String);
  U64 i, begin;
  for (i = 0, begin = 0; i < chopping.len; i++) {
    if (!memory_cmp(chopping.str + i, delimiter.str, delimiter.len)) {
      continue;
    }
    String sliced = slice(chopping, begin, i);
    if (sliced.len) {
      append(tokens, sliced);
    }
    begin = i + delimiter.len;
    // esto sirve para evitarte ciclos innecesarios
    // porque por ejemplo si tienes un separador que es un string largo
    // ejemplo: `SEPARATOR`
    // cuando se encontrase con `SEPARATOR` pues entraria aqui y agarraria al
    // token que esta antes del `SEPARATOR`, y luego iria a la siguiente
    // iteracion donde, se encontraria en EPARATOR, lo cual recorreria hasta
    // encontrar el separator de nuevo, y pues, como actualizamos tambien el
    // begin a que se skipee ya el resto del delimiter pues hay que hacerlo
    // tambien para i :fire:
    i = begin;
  }

  /*if (start < s.len) first impl used this but idk why i needed it jejej*/
  append(tokens, slice(chopping, begin, i));

  return tokens;
}

String *arena_string_split_white_space(Arena *arena, String chopping) {
  String *tokens = arena_array(arena, String);
  U64 i, begin;
  for (i = 0, begin = 0; i < chopping.len; i++) {
    // why does this need to be &&
    // because here we skip if the string is not the delimiter
    // so, when doing this with and every time we, ironically,
    // encounter a ' ' or a \n this if statement gets invalidated,
    // because it needs both statements to be false so we can
    // filter them as parts of the valid words
    // and if one of them is true well it means we encountoured
    // a delimiter
    if (!memory_cmp(chopping.str + i, string("\n").str, 1) && //||
        !memory_cmp(chopping.str + i, string(" ").str, 1)) {
      continue;
    }
    String sliced = slice(chopping, begin, i);
    if (sliced.len) {
      append(tokens, sliced);
    }
    begin = i + 1;
    i = begin;
  }
  // here we added the whole thing as a token because the or made it to continue
  // always because it never could be both of those two conditions at the same
  // time
  append(tokens, slice(chopping, begin, i));
  return tokens;
}

#define MAX_CHAR_LINE 512
#define FILE_SIZE_IN_BYTES 128

String arena_read_file(Arena *arena, const char *file_name) {
  char read_buf[MAX_CHAR_LINE];
  FILE *file = fopen(file_name, "r");
  ARENA_ASSERT(file);
  String content = arena_new_empty_string_with_cap(arena, FILE_SIZE_IN_BYTES);
  while (fgets(read_buf, MAX_CHAR_LINE, file)) {
    arena_c_string_concat(arena, &content, read_buf);
  }
  return content;
}

String arena_string_realloc(Arena *arena, String source) {
  String str = arena_new_empty_string_with_cap(arena, source.cap * 2);
  string_copy(&str, source);
  return str;
}

void arena_string_ptr_realloc_with_cap(Arena *arena, String *source,
                                       U64 new_cap) {
  byte *new_string_memory = arena_alloc(arena, new_cap);
  memory_copy(new_string_memory, source->str, source->len);
  // here we should free the source->str but no... it feels weird rite
  source->str = new_string_memory;
  source->cap = new_cap;
}

// horrible name bro
String *string_split(String *tokens, String chopping, String delimiter) {
  U64 i, begin;
  for (i = 0, begin = 0; i < chopping.len; i++) {
    if (!memory_cmp(chopping.str + i, delimiter.str, delimiter.len)) {
      continue;
    }
    String sliced = slice(chopping, begin, i);
    if (sliced.len) {
      append(tokens, sliced);
    }
    begin = i + delimiter.len;
    i = begin;
  }

  append(tokens, slice(chopping, begin, i));

  return tokens;
}

//

bool c_string_equals(const char *this, const char *that) {
  char this_chr, that_chr;
  do {
    this_chr = *this++;
    that_chr = *that++;
    if (!this_chr) {
      return !(this_chr - that_chr);
    }
  } while (this_chr == that_chr);
  return !(this_chr - that_chr);
}

bool string_equals(String this, String that) {
  if (this.len != that.len) {
    return false;
  }

  for (I64 i = 0; i < this.len; i++) {
    if (this.str[i] != that.str[i]) {
      return false;
    }
  }

  return true;
}

bool memory_cmp(byte *this, byte *that, U64 length) {
  for (U64 i = 0; i < length; i++) {
    if (this[i] != that[i]) {
      return false;
    }
  }
  return true;
}

static inline bool is_white_space(char c) {
  return c == ' ' || c == '\t' || c == '\n';
}

/*
 * string copying
 * */

void c_string_copy(String *to, const char *from) {
  U64 i = 0;
  for (; i < *from && i < to->cap; from++, i++) {
    to->str[i] = *from;
  }
  to->len = i;
}

void string_copy(String *to, String from) {
  U64 i = 0;
  for (; i < from.len; i++) {
    to->str[i] = from.str[i];
  }
  to->len = from.len;
}

void arena_c_string_concat(Arena *arena, String *dest, const char *source) {
  U64 source_len = str_len(source);
  U64 total_size = dest->len + source_len;
  if (total_size > dest->cap) {
    arena_string_ptr_realloc_with_cap(arena, dest, total_size * 1.5);
  }
  memory_copy(dest->str + dest->len, source, source_len);
  dest->len = total_size;
}

// make an arena version of this so if the capacity is overflowed we just
// realloc the content of the string
void string_concat(String *dest, String source) {
  U64 total_size = dest->len + source.len;
  U64 n_bytes = source.len;
  if (total_size >= dest->cap) {
    n_bytes = dest->cap - dest->len;
    total_size = dest->len + n_bytes;
  }
  /*ARENA_ASSERT(total_size < dest->cap);*/
  memory_copy(dest->str + dest->len, source.str, n_bytes);
  dest->len = total_size;
}

void print_string(String str) {
  printf("======================================\n");
  printf("ptr: %p\n\nlen: %lu\ncap: %lu", str.str, str.len, str.cap);
  printfln("str: %S", str.str);
}

/*
 * trimming spaces
 * */

void string_trim_space_left(String *trimming) {
  U64 space_count = 0;
  while (space_count < trimming->len &&
         is_white_space(trimming->str[space_count])) {
    space_count++;
  }
  trimming->str = trimming->str + space_count;
  trimming->len -= space_count;
}

void string_trim_space_right(String *trimming) {
  I64 right_index = trimming->len - 1;
  while (right_index > 0 && is_white_space(trimming->str[right_index])) {
    right_index--;
  }
  trimming->len = right_index + 1;
}

void string_trim_space(String *trimming) {
  string_trim_space_left(trimming);
  string_trim_space_right(trimming);
}

/*
 * trimming  with predicator
 *
 * */
void string_trim_predicator_right(String *trimming,
                                  TrimmerPredicator predicator) {
  U64 right_index = trimming->len - 1;
  while (predicator(trimming->str[right_index])) {
    right_index--;
  }
  trimming->len = right_index + 1;
}

void string_trim_predicator_left(String *trimming,
                                 TrimmerPredicator predicator) {
  U64 predicator_count = 0;
  while (predicator(trimming->str[predicator_count])) {
    predicator_count++;
  }
  trimming->str = trimming->str + predicator_count;
  trimming->len -= predicator_count;
}

void string_trim_predicator(String *trimming, TrimmerPredicator predicator) {
  string_trim_predicator_right(trimming, predicator);
  string_trim_predicator_left(trimming, predicator);
}

// printf with our strings
int printfln(const char *str, ...) {
  va_list ptr;
  va_start(ptr, str);

  char token[1000];
  int k = 0;

  for (int i = 0; str[i] != '\0'; i++) {
    token[k++] = str[i];
    // cortando el token en el momento que hiteamos %
    // nos asegura un state permanente
    // el cual es que, el siguiente token
    // pues tendra el % obvi pero, siempre
    // sera en la posicion 0 del token
    if (str[i + 1] == '%' || str[i + 1] == '\0') {
      // usar la comparacion del null character
      // nos permite imprimir el ultimo  token hasta el, debido a que
      // no se encontraron mas para formatear, pues se tiene que imprimir
      // ese
      // en nuestro caso este es el token %S =====
      token[k] = '\0';
      k = 0;
      if (token[0] != '%') {
        fprintf(stdout, "%s", token);
      } else {
        int j = 1;
        char ch1 = 0;

        while ((ch1 = token[j++]) < 58) {
        }
        if (ch1 == 'i' || ch1 == 'd' || ch1 == 'u' || ch1 == 'h') {
          fprintf(stdout, token, va_arg(ptr, int));
        } else if (ch1 == 'c') {
          fprintf(stdout, token, va_arg(ptr, int));
          // a good refactor would be to just have this if and an else, where it
          // fallstrhouth the vanilla c formatting, so adding stuff is not as
          // noisy
        } else if (ch1 == 'S') {
          String string = va_arg(ptr, String);
          char buf[512];
          memory_set(buf, 512, 0);
          U64 index, i;
          for (index = 0, i = 0; index < string.len; index++) {
            printf("%c", string.str[index]);
            // tofix this please
            /*if (string.str[index] == '\n') {*/
            /*  buf[i] = '\n';*/
            /*  buf[i + 1] = '\0';*/
            /*  fprintf(stdout, "%s", buf);*/
            /*  i = 0;*/
            /*  memory_set(buf, 512, 0);*/
            /*} else {*/
            /*  buf[i++] = string.str[index];*/
            /*}*/
          }
          fprintf(stdout, "%s", token + 2);
        } else if (ch1 == 'b') {
          int boolean = va_arg(ptr, int);
          if (boolean) {
            fprintf(stdout, "%s", "true");
          } else {
            fprintf(stdout, "%s", "false");
          }
          fprintf(stdout, "%s", token + 2);
        } else if (ch1 == 'f') {
          fprintf(stdout, token, va_arg(ptr, double));
        } else if (ch1 == 'l') {
          char ch2 = token[2];
          if (ch2 == 'u' || ch2 == 'd' || ch2 == 'i') {
            fprintf(stdout, token, va_arg(ptr, long));
          } else if (ch2 == 'f') {
            fprintf(stdout, token, va_arg(ptr, double));
          }
        } else if (ch1 == 'L') {
          char ch2 = token[2];
          if (ch2 == 'u' || ch2 == 'd' || ch2 == 'i') {
            fprintf(stdout, token, va_arg(ptr, long long));
          } else if (ch2 == 'f') {
            fprintf(stdout, token, va_arg(ptr, long double));
          }
        } else if (ch1 == 's') {
          fprintf(stdout, token, va_arg(ptr, char *));
        } else {
          fprintf(stdout, "%s", token);
        }
      }
    }
  }
  fprintf(stdout, "\n");
  // ending traversal
  va_end(ptr);
  return 0;
}

#endif /* ifndef _ARENA_STRINGS_H */
