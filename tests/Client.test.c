#include "Client.SFIX.h"
#include <criterion/criterion.h>

#define T SFIX_Client

static T *client = NULL;

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
  client = SFIX_client_constructor();
  /*#endregion*/
}

static void teardown(void) {
  /*#region*/
  client->destructor(client);
  /*#endregion*/
}

Test(T, compose, .fini = teardown, .init = setup) {
  /*#region*/
  SFIX_Pair order[4];
  char *msg;
  createOrder(order, "10", "1.25", "sell");
  int len = sizeof(order) / sizeof(order[0]);
  msg = client->compose(client, 'd', order, len);
  cr_expect_not_null(msg);
  /*#endregion*/
}
