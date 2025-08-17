#include "Candle.SFIX.h"
#include "Integrity.SFIX.h"
#include "PairConvertor.SFIX.h"
#include "Symbol.SFIX.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define T SFIX_Candle

static int __toSfix(T *self, char out[SFIX_MSG_SIZE(SFIX_CANDLE_PROPERTY_LEN)]);
static void __print(T *self, bool showLabels);
static SFIX_Check __checks[SFIX_TAGSNUM] = {
    /*#region*/
    [SFIX_Tag_Date] = {SFIX_is_valid_formatted_date},
    [SFIX_Tag_Time] = {SFIX_is_valid_formatted_time},
    [SFIX_Tag_Timestamp] = {SFIX_is_valid_integer},
    [SFIX_Tag_Open] = {SFIX_is_valid_double},
    [SFIX_Tag_High] = {SFIX_is_valid_double},
    [SFIX_Tag_Low] = {SFIX_is_valid_double},
    [SFIX_Tag_Close] = {SFIX_is_valid_double},
    [SFIX_Tag_Up_volume] = {SFIX_is_valid_integer},
    [SFIX_Tag_Down_volume] = {SFIX_is_valid_integer},
    [SFIX_Tag_Total_volume] = {SFIX_is_valid_integer},
    [SFIX_Tag_Up_ticks] = {SFIX_is_valid_integer},
    [SFIX_Tag_Down_ticks] = {SFIX_is_valid_integer},
    [SFIX_Tag_Total_ticks] = {SFIX_is_valid_integer},
    /*#endregion*/
};

T SFIX_candle_constructor(SFIX_KeyValue pairs[SFIX_TAGSNUM]) {
  /*#region*/
  T candle = {};
  strcpy(candle.date, pairs[SFIX_Tag_Date].value);
  strcpy(candle.time, pairs[SFIX_Tag_Time].value);
  candle.timestamp = strtol(pairs[SFIX_Tag_Timestamp].value, NULL, 10);
  candle.open = atof(pairs[SFIX_Tag_Open].value);
  candle.high = atof(pairs[SFIX_Tag_High].value);
  candle.low = atof(pairs[SFIX_Tag_Low].value);
  candle.close = atof(pairs[SFIX_Tag_Close].value);
  candle.upVolume = atoi(pairs[SFIX_Tag_Up_volume].value);
  candle.downVolume = atoi(pairs[SFIX_Tag_Down_volume].value);
  candle.totalVolume = atoi(pairs[SFIX_Tag_Total_volume].value);
  candle.upTicks = atoi(pairs[SFIX_Tag_Up_ticks].value);
  candle.downTicks = atoi(pairs[SFIX_Tag_Down_ticks].value);
  candle.totalTicks = atoi(pairs[SFIX_Tag_Total_ticks].value);
  candle.print = __print;
  candle.toSfix = __toSfix;
  return candle;
  /*#endregion*/
}

bool SFIX_candle_validate(SFIX_KeyValue pairs[SFIX_TAGSNUM]) {
  /*#region*/
  return SFIX_integrity_validator(pairs, __checks);
  /*#endregion*/
}

static void __print(T *self, bool showLabels) {
  /*#region*/
  char msg[SFIX_MSG_SIZE(8)] = {0};
  __toSfix(self, msg);
  SFIX_print(msg, showLabels);
  /*#endregion*/
}

static int __toSfix(T *self,
                    char out[SFIX_MSG_SIZE(SFIX_CANDLE_PROPERTY_LEN)]) {
  /*#region*/
  SFIX_Pair pairs[SFIX_CANDLE_PROPERTY_LEN] = {0};
  int i = 0;
  SFIX_string_to_string(self->date, SFIX_Tag_Date, &pairs[i++]);
  SFIX_string_to_string(self->time, SFIX_Tag_Time, &pairs[i++]);
  SFIX_time_to_timestamp(self->timestamp, SFIX_Tag_Timestamp, &pairs[i++]);
  SFIX_double_to_string(self->open, SFIX_Tag_Open, &pairs[i++]);
  SFIX_double_to_string(self->high, SFIX_Tag_High, &pairs[i++]);
  SFIX_double_to_string(self->low, SFIX_Tag_Low, &pairs[i++]);
  SFIX_double_to_string(self->close, SFIX_Tag_Close, &pairs[i++]);
  SFIX_int_to_string(self->upVolume, SFIX_Tag_Up_volume, &pairs[i++]);
  SFIX_int_to_string(self->downVolume, SFIX_Tag_Down_volume, &pairs[i++]);
  SFIX_int_to_string(self->totalVolume, SFIX_Tag_Total_volume, &pairs[i++]);
  SFIX_int_to_string(self->upTicks, SFIX_Tag_Up_ticks, &pairs[i++]);
  SFIX_int_to_string(self->downTicks, SFIX_Tag_Down_ticks, &pairs[i++]);
  SFIX_int_to_string(self->totalTicks, SFIX_Tag_Total_ticks, &pairs[i++]);

  int msgLength =
      SFIX_compose(out, SFIX_Symbol_Candle, pairs, SFIX_CANDLE_PROPERTY_LEN);
  if (msgLength <= 0) {
    // LOG_ERROR("Unable to compose candle message");
    return 1;
  }
  return 0;
  /*#endregion*/
}
