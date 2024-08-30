#include "Client.SFIX.h"
#include <criterion/criterion.h>
#include <sfix.h>
#define T Client
#define PROPERTY_LEN 4

#define BUFF(size)                                                             \
  int _listSize = SFIX_estimateMessageSize(size * PROPERTY_LEN);               \
  char _list[_listSize];                                                       \
  memset(_list, 0, _listSize);                                                 \

void createOrder(SFIX_Pair input[PROPERTY_LEN], char *qty, char *price,
                 char *side) {
  /*#region*/
  input[0] = (SFIX_Pair){SFIX_Tag_Quantity, qty};
  input[1] = (SFIX_Pair){SFIX_Tag_Price, price};
  input[2] = (SFIX_Pair){SFIX_Tag_Side, side};
  input[3] = (SFIX_Pair){SFIX_Tag_Symbol, "BTC"};
  /*#endregion*/
}

void createMessage(int size, char out[]) {
  /*#region*/
  for (int i = 0; i < size; i++) {
    SFIX_Pair orders[4];
    createOrder(&orders[0], "10", "1.25", "sell");
    SFIX_compose(out, 'o', orders, PROPERTY_LEN);
  }
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
  SFIX_Pair order[PROPERTY_LEN];
  createOrder(order, "10", "1.25", "sell");
  int len = sizeof(order) / sizeof(order[0]);
  int size = SFIX_estimateMessageSize(PROPERTY_LEN);
  char msg[size];
  SFIX_compose(msg, 'd', order, len);
  cr_expect_not_null(msg);
  /*#endregion*/
}

Test(T, acknowledge, .fini = teardown, .init = setup) {
  /*#region*/
  char msg[248] = {0};
  SFIX_acknowledge(msg);
  char expected[] = "8=SFIX.1\x01"
                    "9=000\x01"
                    "35=A\x01\x02";
  cr_expect_eq(strcmp(msg, expected), 0);
  /*#endregion*/
}

Test(T, unknown, .fini = teardown, .init = setup) {
  /*#region*/
  char msg[248] = {0};
  SFIX_unknown(msg);
  char expected[] = "8=SFIX.1\x01"
                    "9=000\x01"
                    "35=U\x01\x02";
  cr_expect_eq(strcmp(msg, expected), 0);
  /*#endregion*/
}

Test(T, unknown_msg_is_not_a_list, .fini = teardown, .init = setup) {
  /*#region*/
  char msg[248] = {0};
  SFIX_unknown(msg);
  int list_len = SFIX_listLength(msg);
  cr_expect_eq(list_len, 1);
  /*#endregion*/
}

Test(T, retrieve_list_length, .fini = teardown, .init = setup) {
  /*#region*/
  int size = 2;
  BUFF(size);
  createMessage(size, _list);
  int length = SFIX_listLength(_list);
  cr_expect_eq(length, size);
  /*#endregion*/
}

Test(T, parse_single_message, .fini = teardown, .init = setup) {
  /*#region*/
  int size = 1;
  BUFF(size);
  createMessage(size, _list);
  SFIX_KeyValue pairs[256] = {0};
  int status = SFIX_parse(_list, pairs);
  cr_expect_eq(status, 0);
  cr_expect_eq(strcmp(pairs[SFIX_Tag_Quantity].value, "10"), 0);
  cr_expect_eq(strcmp(pairs[SFIX_Tag_Price].value, "1.25"), 0);
  cr_expect_eq(strcmp(pairs[SFIX_Tag_Side].value, "sell"), 0);
  cr_expect_eq(strcmp(pairs[SFIX_Tag_Symbol].value, "BTC"), 0);
  /*#endregion*/
}

Test(T, parse_multiple_messages, .fini = teardown, .init = setup) {
  /*#region*/
  int size = 2;
  BUFF(size);
  createMessage(size, _list);
  SFIX_print(_list);
  SFIX_KeyValue pairs[512] = {0};
  int status = SFIX_parse(_list, pairs);
  cr_expect_eq(status, 0);
  for (int i = 0; i < size; i++) {
    int factor = 256 * i;
    cr_expect_eq(strcmp(pairs[SFIX_Tag_Quantity + factor].value, "10"), 0);
    cr_expect_eq(strcmp(pairs[SFIX_Tag_Price + factor].value, "1.25"), 0);
    cr_expect_eq(strcmp(pairs[SFIX_Tag_Side + factor].value, "sell"), 0);
    cr_expect_eq(strcmp(pairs[SFIX_Tag_Symbol + factor].value, "BTC"), 0);
  }
  /*#endregion*/
}
