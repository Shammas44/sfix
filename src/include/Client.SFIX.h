#ifndef CLIENT_SFIX_H
#define CLIENT_SFIX_H
#ifdef __EMSCRIPTEN__
    // If compiling with Emscripten, __EMSCRIPTEN__ will be defined.
    // In this case, include emscripten.h which properly defines EMSCRIPTEN_KEEPALIVE.
    #include <emscripten.h>
#else
    // If not compiling with Emscripten (e.g., native desktop build),
    // define EMSCRIPTEN_KEEPALIVE as an empty macro so it's ignored by other compilers.
    #ifndef EMSCRIPTEN_KEEPALIVE
        #define EMSCRIPTEN_KEEPALIVE
    #endif
#endif
#include "Tag.SFIX.h"
#include <stdbool.h>

#define T SFIX_Client
#define SFIX_MAX_TAG_LEN 16
#define SFIX_MAX_VALUE_LEN 64
#define SFIX_TAGSNUM 256

#define SFIX_MSG_SIZE(pairsLength)                                             \
  ((pairsLength + 3) * (SFIX_MAX_VALUE_LEN + SFIX_MAX_TAG_LEN + 2) + 2)

typedef struct {
  SFIX_Tag key;
  char value[64]; // replace this by char[64]
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
void SFIX_print(char *message, bool showLabels);


/**
 * Pretty Convert to json string a SFIX message
 *   @param message      An SFIX message
 *   @return             void
 */
int SFIX_toJson(char *message, bool showLabels, char *out_buffer, size_t out_buffer_size);

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

/**
 * Create a set strategy type message
 *   @param name         Name of the strategy
 *   @param out          Addrese where to write the messag
 *   @return             Length of the message
 */
int SFIX_strategy(const char name[64], char out[SFIX_MSG_SIZE(1)]);

/**
 * Create an error type message
 *   @param status       Error status code
 *   @param description  Error description
 *   @return             Length of the message
 */
int SFIX_error(int status, const char description[64],
               char out[SFIX_MSG_SIZE(2)]);

#undef T
#endif
