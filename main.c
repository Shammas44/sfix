#include "Client.SFIX.h"
#include <sfix.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void createOrder(SFIX_Pair input[4], char *qty, char *price, char *side) {
  input[0] = (SFIX_Pair){SFIX_Tag_Quantity, qty};
  input[1] = (SFIX_Pair){SFIX_Tag_Price, price};
  input[2] = (SFIX_Pair){SFIX_Tag_Side, side};
  input[3] = (SFIX_Pair){SFIX_Tag_Symbol, "BTC"};
}

void printOrderList(SFIX_KeyValue *arr, int len) {
  for (int i = 0; i < len; i++) {
    int index = i * 256;
    printf("\n=== order \n");
    printf("quanity: %s\n", arr[SFIX_Tag_Quantity + index].value);
    printf("price: %s\n", arr[SFIX_Tag_Price + index].value);
    printf("side: %s\n", arr[SFIX_Tag_Side + index].value);
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
  createOrder(&orders[0], "10", "1.25", "sell");
  createOrder(&orders[4], "5", "1.5", "buy");
  int len = sizeof(orders) / sizeof(orders[0]);
  int listSize = SFIX_estimateMessageSize(orders, len);
  char list[listSize];
  memset(list, 0, listSize);
  SFIX_composeInnerMessage(list, 2, 4, orders);

  SFIX_Pair pairs[] = {
      {SFIX_Tag_list_length, "2"},
      {SFIX_Tag_List, list},
  };

  int msgSize = SFIX_estimateMessageSize(pairs, 2);
  char msg[msgSize];
  memset(msg, 0, msgSize);
  SFIX_compose(msg, 'o', pairs, sizeof(pairs) / sizeof(pairs[0]));
  printf("\n=== list message \n%s\n",msg);

  // 🧰 parse list message
  // =========================================================================="
  int size = SFIX_isList(msg);
  SFIX_KeyValue *n = calloc(0,sizeof(SFIX_KeyValue) * 256 * size);
  SFIX_parse(msg, n);
  printOrderList(n, size);
  free(n);
}
