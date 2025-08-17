#ifndef UUID_SFIX_H
#define UUID_SFIX_H
#define SFIX_UUID_LENGTH 32
#include <stdbool.h>
#include <stddef.h>
#include <uuid/uuid.h>
#define T SFIX_Uuid

typedef struct T T;

struct T {
  void (*get)(T self, char buffer[SFIX_UUID_LENGTH]);
  bool (*compare)(T self, T x);
  char __value[SFIX_UUID_LENGTH];
};

T SFIX_uuid_constructor(void);
T SFIX_uuid_by_string_constructor(char id[SFIX_UUID_LENGTH]);
void SFIX_uuid_by_custom_length_string(char *buffer, size_t buffer_size,
                                       size_t desired_length);

#undef T
#endif
