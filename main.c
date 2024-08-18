#include "Client.SFIX.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void createOrder(SFIX_Pair input[8], char *qty, char *price) {
  /*#region*/
  int i = 0;
  input[i++] = (SFIX_Pair){SFIX_Tag_Timestamp, "100000"};
  input[i++] = (SFIX_Pair){SFIX_Tag_Client_order_id, "id"};
  input[i++] = (SFIX_Pair){SFIX_Tag_Order_type, "market"};
  input[i++] = (SFIX_Pair){SFIX_Tag_Order_status, "pending"};
  input[i++] = (SFIX_Pair){SFIX_Tag_Side, "buy"};
  input[i++] = (SFIX_Pair){SFIX_Tag_Quantity, qty};
  input[i++] = (SFIX_Pair){SFIX_Tag_Price, price};
  input[i++] = (SFIX_Pair){SFIX_Tag_Symbol, "BTC"};
  /*#endregion*/
}

void printOrderList(SFIX_KeyValue *arr, int len) {
  for (int i = 0; i < len; i++) {
    int index = i * 256;
    printf("\n=== order \n");
    printf("timestamp: %s\n", arr[SFIX_Tag_Timestamp + index].value);
    printf("id: %s\n", arr[SFIX_Tag_Client_order_id + index].value);
    printf("type: %s\n", arr[SFIX_Tag_Order_type + index].value);
    printf("status: %s\n", arr[SFIX_Tag_Order_status + index].value);
    printf("side: %s\n", arr[SFIX_Tag_Side + index].value);
    printf("quanity: %s\n", arr[SFIX_Tag_Quantity + index].value);
    printf("price: %s\n", arr[SFIX_Tag_Price + index].value);
    printf("symbol: %s\n", arr[SFIX_Tag_Symbol + index].value);
  }
}

int main() {
  // 🧰 Compose acknowledge message
  // =========================================================================="
  char acknowledge[83];
  SFIX_acknowledge(acknowledge);
  printf("=== acknowledge \n%s\n\n",acknowledge);

  // 🧰 Compose unknown message
  // =========================================================================="
  char unknown[83];
  SFIX_acknowledge(unknown);
  printf("=== unknown \n%s\n\n",unknown);

  // 🧰 Pretty print message
  // =========================================================================="
  printf("=== pretty print \n");
  SFIX_print(unknown);

  // 🧰 Compose list message
  // =========================================================================="
  SFIX_Pair orders[8] = {0};
  createOrder(&orders[0], "10", "1.25");
  createOrder(&orders[8], "5", "1.5");
  int len = sizeof(orders) / sizeof(orders[0]);
  int listSize = SFIX_estimateMessageSize(orders, len * 2);
  char list[listSize];
  memset(list, 0, listSize);
  SFIX_composeInnerMessage(list, 2, 8, orders);

  SFIX_Pair pairs[] = {
      {SFIX_Tag_list_length, "2"},
      {SFIX_Tag_List, list},
  };

  int pairsLength = sizeof(pairs) / sizeof(pairs[0]);
  int msgSize = SFIX_estimateMessageSize(pairs, pairsLength);
  char msg[msgSize];
  memset(msg, 0, msgSize);
  SFIX_compose(msg, 'O', pairs, pairsLength);
  printf("\n=== list message \n%s\n",msg);

  // 🧰 parse list message
  // =========================================================================="
  int size = SFIX_isList(msg);
  SFIX_KeyValue *n = calloc(0,sizeof(SFIX_KeyValue) * 256 * size);
  SFIX_parse(msg, n);
  printOrderList(n, size);
  free(n);
}
