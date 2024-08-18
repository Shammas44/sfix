#ifndef CLIENT_SFIX_H
#define CLIENT_SFIX_H
#include "Tag.SFIX.h"
#include <stdbool.h>

#define T SFIX_Client
#define SFIX_MAX_FIELDS 128
#define SFIX_MAX_TAG_LEN 16
#define SFIX_MAX_VALUE_LEN 1024
#define SFIX_TAGSNUM 256

#define SFIX_MSG_SIZE(pairsLength)((pairsLength + 4) * (SFIX_MAX_VALUE_LEN + SFIX_MAX_TAG_LEN + 2))

typedef struct {
  SFIX_Tag key;
  char *value;
} SFIX_Pair;

typedef struct {
  char tag[SFIX_MAX_TAG_LEN];
  char value[SFIX_MAX_VALUE_LEN];
} SFIX_KeyValue;

int SFIX_estimateMessageSize(SFIX_Pair body[], int length);
int SFIX_compose(char out[], char type, SFIX_Pair pairs[], int pairsLength);
int SFIX_composeInnerMessage(char out[] ,int rows, int cols, SFIX_Pair pairs[]);
int SFIX_isList(char *message);
int SFIX_parse(char *message, SFIX_KeyValue *out);
void SFIX_print(char *message);
int SFIX_acknowledge(char out[83]);
int SFIX_unknown(char out[83]);

#undef T
#endif
