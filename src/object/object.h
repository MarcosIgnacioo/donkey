#ifndef _OBJECT_H
#define _OBJECT_H
#include "../arena_strings.c"
#include <stdbool.h>

#define OBJECT_TYPES                                                           \
  X(INTEGER)                                                                   \
  X(BOOLEAN)                                                                   \
  X(DONKEY)

#define X(name) name,
typedef enum { OBJECT_TYPES } ObjectType;
#undef X

#define X(name) (#name),
char *ObjectToString[] = {OBJECT_TYPES};
#undef X

typedef struct {
  I64 value;
} ObjectInteger;

typedef struct {
  bool value;
} ObjectBoolean;

typedef String ObjectDonkey;

ObjectDonkey donkey_panic = (ObjectDonkey){.str = "(dokey)", .len = 7, .cap = 7};

typedef struct {
  ObjectType type;
  union {
    ObjectInteger integer;
    ObjectBoolean boolean;
    ObjectDonkey donkey; // this is null btw
  };
} Object;

#endif // !_OBJECT_H
