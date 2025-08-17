#include "Order.SFIX.h"
#include <memd/memd.h>
#include "OrderFactory.SFIX.h"
#include <criterion/criterion.h>
#define T SFIX_Order

static char msg[2048];

static void createOrder(SFIX_Pair input[SFIX_ORDER_PROPERTY_LEN], char *qty, char *price) {
  /*#region*/
  SFIX_Uuid id = SFIX_uuid_by_string_constructor("aaa");
  int i = 0;
  input[i++] = (SFIX_Pair){SFIX_Tag_Timestamp, "100000"};
  input[i++] = (SFIX_Pair){SFIX_Tag_Date, "2024-01-01"};
  input[i++] = (SFIX_Pair){SFIX_Tag_Time, "08:00:00"};
  char idString[SFIX_UUID_LENGTH] = {0};
  id.get(id, idString);
  sprintf(input[i].value, "%s", idString);
  input[i++].key = SFIX_Tag_Client_order_id;
  input[i++] = (SFIX_Pair){SFIX_Tag_Order_type, "0"};
  input[i++] = (SFIX_Pair){SFIX_Tag_Order_status, "0"};
  input[i++] = (SFIX_Pair){SFIX_Tag_Side, "0"};
  sprintf(input[i].value, "%s", qty);
  input[i++].key = SFIX_Tag_Quantity;
  sprintf(input[i].value, "%s", price);
  input[i++].key = SFIX_Tag_Price;
  input[i++] = (SFIX_Pair){SFIX_Tag_Symbol, "BTC"};
  /*#endregion*/
}

static void setup(void) {
  /*#region*/
  int size = 2;
  for (int i = 0; i < size; i++) {
    SFIX_Pair orders[SFIX_ORDER_PROPERTY_LEN] = {0};
    createOrder(orders, "10", "14.45");
    SFIX_compose(msg, 'O', orders, SFIX_ORDER_PROPERTY_LEN);
  }
  /*#endregion*/
}

static void teardown(void) {
  /*#region*/
  memset(msg, 0, 2048);
  /*#endregion*/
}

Test(T, fix_to_order, .fini = teardown, .init = setup) {
  /*#region*/
  SFIX_KeyValue out[512] = {0};
  int e = SFIX_parse(msg, out);
  cr_expect_eq(e, 0);
  for (int i = 0; i < 2; i++) {
    int index = i * 256;
    T order = SFIX_order_constructor(&out[index]);

    // order.print(&order, false);
    // order.print(&order, true);
    cr_expect_eq(order.timestamp, 100000);

    char idString[SFIX_UUID_LENGTH] = {0};
    order.id.get(order.id, idString);
    cr_expect_eq(strcmp(idString, "aaa"), 0);
    cr_expect_eq(strcmp(order.candleDate, "2024-01-01"), 0);
    cr_expect_eq(strcmp(order.candleTime, "08:00:00"), 0);
    cr_expect_eq(order.side, 0);
    cr_expect_eq(order.type, 0);
    cr_expect_eq(order.status, 0);
    cr_expect_eq(order.price, 14.45);
    cr_expect_eq(order.size, 10);
    cr_expect_eq(strcmp(order.symbol, "BTC"), 0);
  }
  /*#endregion*/
}

Test(T, order_to_fix, .fini = teardown) {
  /*#region*/
  int e = 0;
  SFIX_Pair orders[SFIX_ORDER_PROPERTY_LEN] = {0};
  createOrder(orders, "10", "14.45");
  SFIX_compose(msg, 'O', orders, SFIX_ORDER_PROPERTY_LEN);
  SFIX_KeyValue out[256] = {0};
  e = SFIX_parse(msg, out);
  cr_expect_eq(e, 0);
  T order = SFIX_order_constructor(&out[0]);
  char buffer[904] = {0};
  order.toSfix(&order, buffer);
  // order.print(&order, true);
  SFIX_KeyValue out2[256] = {0};
  e = SFIX_parse(buffer, out2);
  T order2 = SFIX_order_constructor(&out2[0]);
  cr_expect_eq(order2.timestamp, 100000);

  char idString[SFIX_UUID_LENGTH] = {0};
  order.id.get(order.id, idString);
  cr_expect_eq(strcmp(idString, "aaa"), 0);
  cr_expect_eq(strcmp(order.candleDate, "2024-01-01"), 0);
  cr_expect_eq(strcmp(order.candleTime, "08:00:00"), 0);
  cr_expect_eq(order2.side, 0);
  cr_expect_eq(order2.type, 0);
  cr_expect_eq(order2.status, 0);
  cr_expect_eq(order2.price, 14.45);
  cr_expect_eq(order2.size, 10);
  cr_expect_eq(strcmp(order2.symbol, "BTC"), 0);
  /*#endregion*/
}

Test(T, id_length, .fini = teardown) {
  /*#region*/
  SFIX_OrderFactory factory = SFIX_orderFactory_constructor("USD");
  SFIX_Order order = {0};

  char k[32] = "aaaaaaaaaa-bbbbbbbbbb-cccccccccc";
  factory.init(&factory, &order)
    ->setId(&factory, SFIX_uuid_by_string_constructor(k))
    ->setSide(&factory, SFIX_ORDER_SIDE_BUY)
    ->build(&factory);
  size_t size = SFIX_estimateMessageSize(SFIX_ORDER_PROPERTY_LEN);
  char buffer[size];
  memset(buffer, 0x00, size);
  order.toSfix(&order, buffer);
  SFIX_KeyValue out[256] = {0};
  int e = SFIX_parse(buffer, out);
  cr_expect_eq(e, 0);
  /*#endregion*/
}

Test(T, acknowledge_msg_should_be_invalid, .fini = teardown) {
  /*#region*/
  char msg[248] = {0};
  SFIX_acknowledge(msg);
  SFIX_KeyValue out[256] = {0};
  SFIX_parse(msg, out);
  cr_expect_eq(SFIX_order_validate(out), false);
  /*#endregion*/
}

Test(T, order_validate_wrong_date_format, .fini = teardown) {
  /*#region*/
  cr_skip();
  // SFIX_Pair input[PROPERTY_LEN] = {};
  // createCandle(input);
  // set wrong date format on purpose
  // input[0] = (SFIX_Pair){SFIX_Tag_Date, "01-01-2024"};
  // SFIX_KeyValue out[256] = {};
  // SFIX_compose(msg, SFIX_Symbol_Candle, input, PROPERTY_LEN);
  // SFIX_parse(msg, out);
  // cr_expect_eq(SFIX_candle_validate(out), false);
  /*#endregion*/
}

Test(T, order_validate_wrong_time_format, .fini = teardown) {
  /*#region*/
  cr_skip();
  // char message[2048];
  // SFIX_Pair input[PROPERTY_LEN] = {};
  // createCandle(input);
  // set wrong time format on purpose
  // input[1] = (SFIX_Pair){SFIX_Tag_Time, "08:00"};
  // SFIX_KeyValue out[256] = {};
  // SFIX_compose(message, SFIX_Symbol_Candle, input, PROPERTY_LEN);
  // SFIX_parse(message, out);
  // cr_expect_eq(SFIX_candle_validate(out), false);
  /*#endregion*/
}

Test(T, order_validate_wrong_integer, .fini = teardown) {
  /*#region*/
  cr_skip();
  // char message[2048];
  // SFIX_Pair input[PROPERTY_LEN] = {};
  // createCandle(input);
  // set wrong up volume format on purpose
  // input[7] = (SFIX_Pair){SFIX_Tag_Up_volume, "12e"};
  // SFIX_KeyValue out[256] = {};
  // SFIX_compose(message, SFIX_Symbol_Candle, input, PROPERTY_LEN);
  // SFIX_parse(message, out);
  // cr_expect_eq(SFIX_candle_validate(out), false);
  /*#endregion*/
}

Test(T, order_validate_wrong_double, .fini = teardown) {
  /*#region*/
  cr_skip();
  // char message[2048];
  // SFIX_Pair input[PROPERTY_LEN] = {};
  // createCandle(input);
  // set wrong open format on purpose
  // input[3] = (SFIX_Pair){SFIX_Tag_Open, "10,5"};
  // SFIX_KeyValue out[256] = {};
  // SFIX_compose(message, SFIX_Symbol_Candle, input, PROPERTY_LEN);
  // SFIX_parse(message, out);
  // cr_expect_eq(SFIX_candle_validate(out), false);
  /*#endregion*/
}
