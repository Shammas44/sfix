#include "Order.SFIX.h"
#include "Integrity.SFIX.h"
#include "PairConvertor.SFIX.h"
#include "Symbol.SFIX.h"
// #include <logger/logger.h>
#include <memd/memd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define T SFIX_Order

static bool __is_correct_message_type(const char str[SFIX_MAX_VALUE_LEN]);
static bool __is_valid_order_type(const char str[SFIX_MAX_VALUE_LEN]);
static bool __is_valid_order_side(const char str[SFIX_MAX_VALUE_LEN]);
static bool __is_valid_order_status(const char str[SFIX_MAX_VALUE_LEN]);
static bool __is_valid_client_order_id(const char str[SFIX_MAX_VALUE_LEN]);
static bool __is_valid_timestamp(const char str[SFIX_MAX_VALUE_LEN]);
static bool __is_valid_symbol(const char str[SFIX_MAX_VALUE_LEN]);
static bool __is_valid_price(const char str[SFIX_MAX_VALUE_LEN]);
static bool __is_valid_size(const char str[SFIX_MAX_VALUE_LEN]);
static int __toSfix(T *self, char out[SFIX_MSG_SIZE(8)]);
static void __print(T *self, bool showLabels);

static SFIX_Check checks[SFIX_TAGSNUM] = {
    [SFIX_Tag_Date] = {SFIX_is_valid_formatted_date},
    [SFIX_Tag_Time] = {SFIX_is_valid_formatted_time},
    [SFIX_Tag_Client_order_id] = {__is_valid_client_order_id},
    [SFIX_Tag_Message_type] = {__is_correct_message_type},
    [SFIX_Tag_Timestamp] = {__is_valid_timestamp},
    [SFIX_Tag_Order_type] = {__is_valid_order_type},
    [SFIX_Tag_Order_status] = {__is_valid_order_status},
    [SFIX_Tag_Side] = {__is_valid_order_side},
    [SFIX_Tag_Quantity] = {__is_valid_size},
    [SFIX_Tag_Price] = {__is_valid_price},
    [SFIX_Tag_Symbol] = {__is_valid_symbol},
};

T SFIX_order_constructor(SFIX_KeyValue pairs[SFIX_TAGSNUM]) {
  // #region
  T order = {};
  SFIX_Uuid id = SFIX_uuid_by_string_constructor(pairs[11].value);
  order.id = id;
  order.timestamp = strtol(pairs[SFIX_Tag_Timestamp].value, NULL, 10);
  strcpy(order.candleDate, pairs[SFIX_Tag_Date].value);
  strcpy(order.candleTime, pairs[SFIX_Tag_Time].value);
  order.type = atoi(pairs[SFIX_Tag_Order_type].value);
  order.status = atoi(pairs[SFIX_Tag_Order_status].value);
  order.side = atoi(pairs[SFIX_Tag_Side].value);
  order.size = atof(pairs[SFIX_Tag_Quantity].value);
  order.price = atof(pairs[SFIX_Tag_Price].value);
  strncpy(order.symbol, pairs[SFIX_Tag_Symbol].value, 8 - 1);
  order.symbol[8 - 1] = '\0';
  order.print = __print;
  order.toSfix = __toSfix;
  return order;
  // #endregion
}

static int __toSfix(T *self, char out[SFIX_MSG_SIZE(8)]) {
  /*#region*/
  SFIX_Pair pairs[SFIX_ORDER_PROPERTY_LEN] = {0};
  int i = 0;
  char id[SFIX_UUID_LENGTH] = {0};
  self->id.get(self->id, id);
  SFIX_string_to_string(id, SFIX_Tag_Client_order_id, &pairs[i++]);
  SFIX_time_to_timestamp(self->timestamp, SFIX_Tag_Timestamp, &pairs[i++]);
  SFIX_string_to_string(self->candleDate, SFIX_Tag_Date, &pairs[i++]);
  SFIX_string_to_string(self->candleTime, SFIX_Tag_Time, &pairs[i++]);
  SFIX_int_to_string(self->type, SFIX_Tag_Order_type, &pairs[i++]);
  SFIX_int_to_string(self->status, SFIX_Tag_Order_status, &pairs[i++]);
  SFIX_int_to_string(self->side, SFIX_Tag_Side, &pairs[i++]);
  SFIX_double_to_string(self->size, SFIX_Tag_Quantity, &pairs[i++]);
  SFIX_double_to_string(self->price, SFIX_Tag_Price, &pairs[i++]);
  // check symbol
  SFIX_string_to_string(self->symbol, SFIX_Tag_Symbol, &pairs[i++]);

  int msgLength =
      SFIX_compose(out, SFIX_Symbol_Order, pairs, SFIX_ORDER_PROPERTY_LEN);
  if (msgLength <= 0) {
    // LOG_ERROR("Unable to compose order message");
    return 1;
  }
  return 0;
  /*#endregion*/
}

static void __print(T *self, bool showLabels) {
  /*#region*/
  char msg[SFIX_MSG_SIZE(8)] = {0};
  __toSfix(self, msg);
  SFIX_print(msg, showLabels);
  /*#endregion*/
}

bool SFIX_order_validate(SFIX_KeyValue pairs[SFIX_TAGSNUM]) {
  /*#region*/
  return SFIX_integrity_validator(pairs, checks);
  /*#endregion*/
}

static bool __is_valid_order_type(const char str[SFIX_MAX_VALUE_LEN]) {
  /*#region*/
  int value = atoi(str);
  bool check;
  check = value >= 0 && value < $SFIX_ORDER_TYPE_LAST;
  if (!check)
    fprintf(stderr, "invalid order type");
    // LOG_WARNING("invalid order type");
  return check;
  /*#endregion*/
}

static bool __is_correct_message_type(const char str[SFIX_MAX_VALUE_LEN]) {
  /*#region*/
  char value = str[0];
  bool check;
  check = value == 'O';
  if (!check)
    fprintf(stderr, "invalid message type\n");
    // LOG_WARNING("invalid message type");
  return check;
  /*#endregion*/
}

static bool __is_valid_order_side(const char str[SFIX_MAX_VALUE_LEN]) {
  /*#region*/
  int value = atoi(str);
  bool check;
  check = value >= 0 && value < $SFIX_ORDER_SIDE_LAST;
  if (!check)
    fprintf(stderr, "invalid order side\n");
    // LOG_WARNING("invalid order side");
  return check;
  /*#endregion*/
}

static bool __is_valid_order_status(const char str[SFIX_MAX_VALUE_LEN]) {
  /*#region*/
  int value = atoi(str);
  bool check;
  check = value >= 0 && value < $SFIX_ORDER_STATUS_LAST;
  if (!check)
    fprintf(stderr, "invalid order status\n");
    // LOG_WARNING("invalid order status");
  return check;
  /*#endregion*/
}

static bool __is_valid_client_order_id(const char str[SFIX_MAX_VALUE_LEN]) {
  /*#region*/
  bool check = SFIX_is_non_empty_printable(str);
  if (!check)
    fprintf(stderr, "invalid client order id\n");
    // LOG_WARNING("invalid client order id");
  return check;
  /*#endregion*/
}

static bool __is_valid_timestamp(const char str[SFIX_MAX_VALUE_LEN]) {
  /*#region*/
  bool check = SFIX_is_valid_integer(str);
  if (!check)
    fprintf(stderr, "invalid timestamp\n");
    // LOG_WARNING("invalid timestamp");
  return check;
  /*#endregion*/
}

static bool __is_valid_symbol(const char str[SFIX_MAX_VALUE_LEN]) {
  /*#region*/
  bool check = SFIX_is_valid_ticker(str);
  if (!check)
    fprintf(stderr, "invalid symbol\n");
    // LOG_WARNING("invalid symbol");
  return check;
  /*#endregion*/
}

static bool __is_valid_price(const char str[SFIX_MAX_VALUE_LEN]) {
  /*#region*/
  bool check = SFIX_is_valid_double(str);
  if (!check)
    // LOG_WARNING("invalid price");
    fprintf(stderr, "invalid price\n");
  return check;
  /*#endregion*/
}

static bool __is_valid_size(const char str[SFIX_MAX_VALUE_LEN]) {
  /*#region*/
  bool check = SFIX_is_valid_double(str);
  if (!check)
    // LOG_WARNING("invalid size");
    fprintf(stderr, "invalid size\n");
  return check;
  /*#endregion*/
}
