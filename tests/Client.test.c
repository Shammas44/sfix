#include "Client.SFIX.h"
#include <criterion/criterion.h>
#include <sfix.h>
#define T Client

void createOrder(SFIX_Pair input[4], char *qty, char *price, char *side) {
  /*#region*/
  input[0] = (SFIX_Pair){SFIX_Tag_Quantity, qty};
  input[1] = (SFIX_Pair){SFIX_Tag_Price, price};
  input[2] = (SFIX_Pair){SFIX_Tag_Side, side};
  input[3] = (SFIX_Pair){SFIX_Tag_Symbol, "BTC"};
  /*#endregion*/
}

static void setup(void) {
  /*#region*/
  /*#endregion*/
}

static void teardown(void) {
  /*#region*/
  /*#endregion*/
}

Test(T, compose, .fini = teardown, .init = setup) {
  /*#region*/
  SFIX_Pair order[4];
  createOrder(order, "10", "1.25", "sell");
  int len = sizeof(order) / sizeof(order[0]);
  int size = SFIX_estimateMessageSize(order,4);
  char msg[size];
  SFIX_compose(msg, 'd', order, len);
  cr_expect_not_null(msg);
  /*#endregion*/
}

Test(T, acknowledge, .fini = teardown, .init = setup) {
  /*#region*/
  char msg[83];
  int size = SFIX_acknowledge(msg);
  cr_expect_not_null(msg);
  cr_expect_eq(size, 28);
  /*#endregion*/
}

Test(T, unknown, .fini = teardown, .init = setup) {
  /*#region*/
  char msg[83] = {0};
  int size = SFIX_unknown(msg);
  cr_expect_not_null(msg);
  cr_expect_eq(size, 28);
  /*#endregion*/
}

Test(T, unknown_msg_is_not_a_list, .fini = teardown, .init = setup) {
  /*#region*/
  char msg[83] = {0};
  SFIX_unknown(msg);
  int list_len = SFIX_isList(msg);
  cr_expect_eq(list_len, -1);
  /*#endregion*/
}

Test(T, parse, .fini = teardown, .init = setup) {
  /*#region*/
  SFIX_Pair order[4];
  createOrder(order, "10", "1.25", "sell");
  int len = sizeof(order) / sizeof(order[0]);
  int size = SFIX_estimateMessageSize(order,4);
  char msg[size];
  SFIX_compose(msg, 'd', order, len);
  SFIX_KeyValue pairs[256] = {0};
  int status = SFIX_parse(msg, pairs);
  cr_expect_eq(status,0);
  cr_expect_eq(strcmp(pairs[SFIX_Tag_Quantity].value, "10"), 0);
  cr_expect_eq(strcmp(pairs[SFIX_Tag_Price].value, "1.25"), 0);
  cr_expect_eq(strcmp(pairs[SFIX_Tag_Side].value, "sell"), 0);
  cr_expect_eq(strcmp(pairs[SFIX_Tag_Symbol].value, "BTC"), 0);
  /*#endregion*/
}

Test(T, composeListMessage, .fini = teardown, .init = setup) {
  /*#region*/
  SFIX_Pair orders[8];
  createOrder(&orders[0], "10", "1.25", "sell");
  createOrder(&orders[4], "10", "1.25", "sell");

  int listSize = SFIX_estimateMessageSize(orders,8);
  char list[listSize];
  memset(list,0,listSize);
  SFIX_composeInnerMessage(list, 2, 4, orders);

  SFIX_Pair pairs[] = {
      {SFIX_Tag_list_length, "2"},
      {SFIX_Tag_List, list},
  };

  int msgSize = SFIX_estimateMessageSize(pairs,2);
  char msg[msgSize];
  memset(msg,0,msgSize);
  SFIX_compose(msg,'o', pairs, sizeof(pairs) / sizeof(pairs[0]));
  /*#endregion*/
}
