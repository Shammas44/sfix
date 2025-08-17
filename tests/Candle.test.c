#include "Candle.SFIX.h"
#include "Symbol.SFIX.h"
#include <criterion/criterion.h>
#define T SFIX_Candle
#define PROPERTY_LEN 13

static char msg[2048];

static void createCandle(SFIX_Pair input[PROPERTY_LEN]) {
  /*#region*/
  int i = 0;
  input[i++] = (SFIX_Pair){SFIX_Tag_Date, "2024-01-01"};
  input[i++] = (SFIX_Pair){SFIX_Tag_Time, "08:00:00"};
  input[i++] = (SFIX_Pair){SFIX_Tag_Timestamp, "100000"};
  input[i++] = (SFIX_Pair){SFIX_Tag_Open, "10.5"};
  input[i++] = (SFIX_Pair){SFIX_Tag_High, "10.5"};
  input[i++] = (SFIX_Pair){SFIX_Tag_Low, "10.5"};
  input[i++] = (SFIX_Pair){SFIX_Tag_Close, "10.5"};
  input[i++] = (SFIX_Pair){SFIX_Tag_Up_volume, "1000"};
  input[i++] = (SFIX_Pair){SFIX_Tag_Down_volume, "1000"};
  input[i++] = (SFIX_Pair){SFIX_Tag_Total_volume, "1000"};
  input[i++] = (SFIX_Pair){SFIX_Tag_Up_ticks, "10"};
  input[i++] = (SFIX_Pair){SFIX_Tag_Down_ticks, "10"};
  input[i++] = (SFIX_Pair){SFIX_Tag_Total_ticks, "10"};
  /*#endregion*/
}

static void checkCandle(T candle) {
  /*#region*/
  cr_expect_eq(strcmp(candle.date, "2024-01-01"), 0);
  cr_expect_eq(strcmp(candle.time, "08:00:00"), 0);
  cr_expect_eq(candle.timestamp, 100000);
  cr_expect_eq(candle.open, 10.5);
  cr_expect_eq(candle.high, 10.5);
  cr_expect_eq(candle.low, 10.5);
  cr_expect_eq(candle.close, 10.5);
  cr_expect_eq(candle.upVolume, 1000);
  cr_expect_eq(candle.downVolume, 1000);
  cr_expect_eq(candle.totalVolume, 1000);
  cr_expect_eq(candle.upTicks, 10);
  cr_expect_eq(candle.downTicks, 10);
  cr_expect_eq(candle.totalTicks, 10);
  /*#endregion*/
}

static void setup(void) {
  /*#region*/
  int size = 2;
  for (int i = 0; i < size; i++) {
    SFIX_Pair candle[PROPERTY_LEN] = {0};
    createCandle(candle);
    SFIX_compose(msg, SFIX_Symbol_Candle, candle, PROPERTY_LEN);
  }
  /*#endregion*/
}

static void teardown(void) {
  /*#region*/
  memset(msg, 0, 2048);
  /*#endregion*/
}

Test(T, fix_to_candle, .fini = teardown, .init = setup) {
  /*#region*/
  SFIX_KeyValue out[512] = {0};
  int e = SFIX_parse(msg, out);
  cr_expect_eq(e, 0);
  for (int i = 0; i < 2; i++) {
    int index = i * 256;
    T candle = SFIX_candle_constructor(&out[index]);
    checkCandle(candle);
  }
  /*#endregion*/
}

Test(T, candle_to_fix, .fini = teardown) {
  /*#region*/
  int e = 0;
  SFIX_Pair candles[PROPERTY_LEN] = {0};
  createCandle(candles);
  SFIX_compose(msg, SFIX_Symbol_Candle, candles, PROPERTY_LEN);
  SFIX_KeyValue out[256] = {0};
  e = SFIX_parse(msg, out);
  cr_expect_eq(e, 0);
  T candle = SFIX_candle_constructor(out);
  char buffer[1314];
  memset(buffer, 0, 1314);
  candle.toSfix(&candle, buffer);
  SFIX_KeyValue pairs[256] = {0};
  e = SFIX_parse(buffer, pairs);
  T expected = SFIX_candle_constructor(&pairs[0]);
  checkCandle(expected);
  /*#endregion*/
}

Test(T, candle_validate, .fini = teardown) {
  /*#region*/
  SFIX_Pair input[PROPERTY_LEN] = {};
  createCandle(input);
  SFIX_KeyValue out[256] = {};
  SFIX_compose(msg, SFIX_Symbol_Candle, input, PROPERTY_LEN);
  SFIX_parse(msg, out);
  cr_expect_eq(SFIX_candle_validate(out), true);
  /*#endregion*/
}

Test(T, candle_validate_wrong_date_format, .fini = teardown) {
  /*#region*/
  SFIX_Pair input[PROPERTY_LEN] = {};
  createCandle(input);
  // set wrong date format on purpose
  input[0] = (SFIX_Pair){SFIX_Tag_Date, "01-01-2024"};
  SFIX_KeyValue out[256] = {};
  SFIX_compose(msg, SFIX_Symbol_Candle, input, PROPERTY_LEN);
  SFIX_parse(msg, out);
  cr_expect_eq(SFIX_candle_validate(out), false);
  /*#endregion*/
}

Test(T, candle_validate_wrong_time_format, .fini = teardown) {
  /*#region*/
  char message[2048];
  SFIX_Pair input[PROPERTY_LEN] = {};
  createCandle(input);
  // set wrong time format on purpose
  input[1] = (SFIX_Pair){SFIX_Tag_Time, "08:00"};
  SFIX_KeyValue out[256] = {};
  SFIX_compose(message, SFIX_Symbol_Candle, input, PROPERTY_LEN);
  SFIX_parse(message, out);
  cr_expect_eq(SFIX_candle_validate(out), false);
  /*#endregion*/
}

Test(T, candle_validate_wrong_integer, .fini = teardown) {
  /*#region*/
  char message[2048];
  SFIX_Pair input[PROPERTY_LEN] = {};
  createCandle(input);
  // set wrong up volume format on purpose
  input[7] = (SFIX_Pair){SFIX_Tag_Up_volume, "12e"};
  SFIX_KeyValue out[256] = {};
  SFIX_compose(message, SFIX_Symbol_Candle, input, PROPERTY_LEN);
  SFIX_parse(message, out);
  cr_expect_eq(SFIX_candle_validate(out), false);
  /*#endregion*/
}

Test(T, candle_validate_wrong_double, .fini = teardown) {
  /*#region*/
  char message[2048];
  SFIX_Pair input[PROPERTY_LEN] = {};
  createCandle(input);
  // set wrong open format on purpose
  input[3] = (SFIX_Pair){SFIX_Tag_Open, "10,5"};
  SFIX_KeyValue out[256] = {};
  SFIX_compose(message, SFIX_Symbol_Candle, input, PROPERTY_LEN);
  SFIX_parse(message, out);
  cr_expect_eq(SFIX_candle_validate(out), false);
  /*#endregion*/
}
