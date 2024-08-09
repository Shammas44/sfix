#ifndef CLIENT_SFIX_H
#define CLIENT_SFIX_H
#include "Tag.SFIX.h"

#define T SFIX_Client
#define SFIX_MAX_FIELDS 128
#define SFIX_MAX_TAG_LEN 16
#define SFIX_MAX_VALUE_LEN 64
#define SFIX_TAGSNUM 256

typedef struct T T;

typedef struct {
  SFIX_Tag key;
  char *value;
} SFIX_Pair;

typedef struct {
  char tag[SFIX_MAX_TAG_LEN];
  char value[SFIX_MAX_VALUE_LEN];
} SFIX_KeyValue;

struct T {
  char *(*compose)(T *self, char type, SFIX_Pair pairs[], int pairsLength, int *out_length);
  void (*destructor)(T *self);
  char *(*composeInnerMessage)(T *self, int message_id, SFIX_Pair pairs[], int pairsLength);
  char *(*concatInnerMessages)(T *self, char *pairs[], int length);
  int (*isList)(T *self, char *message);
  int (*parse)(T *self, char *message, SFIX_KeyValue *out);
  void (*print)(T *self, char *message);
  char *(*acknowledge)(T *self, int *out_length);
  char *(*unknown)(T *self, int *out_length);
  int (*estimateMessageSize)(T*self, int pairsLength);
};

T *SFIX_client_constructor();

#undef T
#endif
