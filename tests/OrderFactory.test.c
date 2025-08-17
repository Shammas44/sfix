#define UNIT_TESTING TRUE
#include "OrderFactory.SFIX.h"
#include <criterion/criterion.h>
#define T SFIX_OrderFactory
#define PROPERTY_LEN 8
#include "Integrity.SFIX.h"

static T factory;

static void setup(void) {
  /*#region*/
  factory = SFIX_orderFactory_constructor("USD");
  /*#endregion*/
}

static void teardown(void) {
  /*#region*/
  // memset(msg, 0, 2048);
  /*#endregion*/
}

Test(T, full_build, .fini = teardown, .init = setup) {
  /*#region*/
  SFIX_Order order;
  factory.init(&factory, &order)
      ->generateId(&factory)
      ->setType(&factory, SFIX_ORDER_TYPE_LIMIT)
      ->setStatus(&factory, SFIX_ORDER_STATUS_PENDING)
      ->setSide(&factory, SFIX_ORDER_SIDE_BUY)
      ->setSize(&factory, 100.5)
      ->setCandleDate(&factory, "2024-01-01")
      ->setCandleTime(&factory, "08:00:00")
      ->setPrice(&factory, 1500.75)
      ->setSymbol(&factory, "BTCUSD")
      ->build(&factory);
  /*#endregion*/
}

Test(T, init, .fini = teardown, .init = setup) {
  /*#region*/
  SFIX_Order order;
  factory.init(&factory, &order)->build(&factory);
  cr_expect_eq(strcmp(order.symbol, "USD"), 0);
  cr_expect_gt(order.timestamp, 0);
  char id[SFIX_UUID_LENGTH] = {0};
  order.id.get(order.id, id);
  cr_expect_eq(__match_string(id, "[a-z0-9-]+"), true);
  /*#endregion*/
}

Test(T, setId, .fini = teardown, .init = setup) {
  /*#region*/
  SFIX_Order order;
  SFIX_Uuid id = SFIX_uuid_by_string_constructor("123");
  factory.init(&factory, &order)->setId(&factory, id)->build(&factory);
  char idString[SFIX_UUID_LENGTH] = {0};
  order.id.get(order.id, idString);
  cr_expect_eq(strcmp(idString, "123"), 0);
  /*#endregion*/
}

Test(T, setType, .fini = teardown, .init = setup) {
  /*#region*/
  SFIX_Order order;
  factory.init(&factory, &order)
      ->setType(&factory, SFIX_ORDER_TYPE_LIMIT)
      ->build(&factory);
  cr_expect_eq(order.type, SFIX_ORDER_TYPE_LIMIT);
  /*#endregion*/
}

Test(T, setStatus, .fini = teardown, .init = setup) {
  /*#region*/
  SFIX_Order order;
  factory.init(&factory, &order)
      ->setStatus(&factory, SFIX_ORDER_STATUS_EXECUTED)
      ->build(&factory);
  cr_expect_eq(order.status, SFIX_ORDER_STATUS_EXECUTED);
  /*#endregion*/
}

Test(T, setSide, .fini = teardown, .init = setup) {
  /*#region*/
  SFIX_Order order;
  factory.init(&factory, &order)
      ->setSide(&factory, SFIX_ORDER_SIDE_SELL)
      ->build(&factory);
  cr_expect_eq(order.side, SFIX_ORDER_SIDE_SELL);
  /*#endregion*/
}

Test(T, setSize, .fini = teardown, .init = setup) {
  /*#region*/
  SFIX_Order order;
  factory.init(&factory, &order)->setSize(&factory, 45.5)->build(&factory);
  cr_expect_eq(order.size, 45.5);
  /*#endregion*/
}

Test(T, setPrice, .fini = teardown, .init = setup) {
  /*#region*/
  SFIX_Order order;
  factory.init(&factory, &order)->setPrice(&factory, 20.5)->build(&factory);
  cr_expect_eq(order.price, 20.5);
  /*#endregion*/
}

Test(T, setSymbol, .fini = teardown, .init = setup) {
  /*#region*/
  SFIX_Order order;
  factory.init(&factory, &order)->setSymbol(&factory, "BTC")->build(&factory);
  cr_expect_eq(strcmp(order.symbol, "BTC"), 0);
  /*#endregion*/
}

Test(T, setCandleDate, .fini = teardown, .init = setup) {
  /*#region*/
  SFIX_Order order;
  factory.init(&factory, &order)->setCandleDate(&factory, "2024-01-01")->build(&factory);
  cr_expect_eq(strcmp(order.candleDate, "2024-01-01"), 0);
  /*#endregion*/
}
Test(T, setCandleTime, .fini = teardown, .init = setup) {
  /*#region*/
  SFIX_Order order;
  factory.init(&factory, &order)->setCandleTime(&factory, "08:00:00")->build(&factory);
  cr_expect_eq(strcmp(order.candleTime, "08:00:00"), 0);
  /*#endregion*/
}

Test(T, generateId, .fini = teardown, .init = setup) {
  /*#region*/
  SFIX_Order order;
  factory.init(&factory, &order)->build(&factory);

  char previousId[SFIX_UUID_LENGTH] = {0};
  order.id.get(order.id, previousId);

  factory.generateId(&factory)->build(&factory);
  char currentId[SFIX_UUID_LENGTH] = {0};
  order.id.get(order.id, currentId);
  cr_expect_neq(strcmp(previousId, currentId), 0);
  cr_expect_eq(__match_string(currentId, "[a-z0-9-]+"), true);
  /*#endregion*/
}
