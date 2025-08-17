#ifndef CANDLE_SFIX_H
#define CANDLE_SFIX_H
#include "Client.SFIX.h"
#include <time.h>
#define T SFIX_Candle
#define SFIX_CANDLE_PROPERTY_LEN 13

typedef struct T T;

struct T {
  time_t timestamp;
  char date[11];
  char time[9];
  double open;
  double high;
  double low;
  double close;
  int upVolume;
  int downVolume;
  int totalVolume;
  int upTicks;
  int downTicks;
  int totalTicks;
  int (*toSfix)(T *self, char out[SFIX_MSG_SIZE(SFIX_CANDLE_PROPERTY_LEN)]);
  void (*print)(T *self, bool showLabels);
};

/**
 * Candle constructor
 *   @param pairs        An array of tag/value pairs
 *   @return             SFIX_Candle
 */
T SFIX_candle_constructor(SFIX_KeyValue pairs[SFIX_TAGSNUM]);

/**
 * Validate the integrity of an array of candle tag/value pairs
 *   @param pairs        An array of tag/value pairs
 *   @return             bool
 */
bool SFIX_candle_validate(SFIX_KeyValue pairs[SFIX_TAGSNUM]);

#undef T
#endif
