#ifndef CLIENT_SFIX_H
#define CLIENT_SFIX_H
#include "Tag.SFIX.h"
#include <stdbool.h>

#define T SFIX_Client
#define SFIX_MAX_TAG_LEN 16
#define SFIX_MAX_VALUE_LEN 64
#define SFIX_TAGSNUM 256

#define SFIX_MSG_SIZE(pairsLength)((pairsLength + 3) * (SFIX_MAX_VALUE_LEN + SFIX_MAX_TAG_LEN + 2) + 2)

typedef struct {
  SFIX_Tag key;
  char *value;
} SFIX_Pair;

typedef struct {
  char tag[SFIX_MAX_TAG_LEN];
  char value[SFIX_MAX_VALUE_LEN];
} SFIX_KeyValue;

/**
 * Retrieve necessary byte size to compose a message of x body pairs
 *   @param pairs_length   Number of body pairs
 *   @return               Size in bytes
 */
int SFIX_estimateMessageSize(int pairs_length);

/**
 * Create a message
 *   @param out          Already allocated buffer to store the message 
 *   @param type         Message type
 *   @param pairs        An array of pairs
 *   @param pairsLength  The number of pairs
 *   @return             Size of the message
 */
int SFIX_compose(char out[], char type, SFIX_Pair pairs[], int pairsLength);

/**
 * Retrieve number of sub messages contained into a SFIX message
 *   @param message      An SFIX message
 *   @return             Length of the message list
 */
int SFIX_listLength(char *message);

/**
 * Parse a message
 *   @param message      An SFIX message
 *   @param out          Structure to store the output
 *   @return             Error code
 */
int SFIX_parse(char *message, SFIX_KeyValue out[]);

/**
 * Pretty print a SFIX message
 *   @param message      An SFIX message
 *   @return             void
 */
void SFIX_print(char *message);

/**
 * Create an acknowledge type message
 *   @param out          addrese where to write the messag
 *   @return             Length of the message
 */
int SFIX_acknowledge(char out[SFIX_MSG_SIZE(0)]);

/**
 * Create an unknown type message
 *   @param out          addrese where to write the messag
 *   @return             Length of the message
 */
int SFIX_unknown(char out[SFIX_MSG_SIZE(0)]);

#undef T
#endif
