#include "Client.SFIX.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROPERTY_LENGTH 8

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
  printf("=== acknowledge \n%s\n\n", acknowledge);

  // 🧰 Compose unknown message
  // =========================================================================="
  char unknown[83];
  SFIX_acknowledge(unknown);
  printf("=== unknown \n%s\n\n", unknown);

  // 🧰 Pretty print message
  // =========================================================================="
  printf("=== pretty print \n");
  SFIX_print(unknown);

  // 🧰 Compose list message
  // =========================================================================="
  int size = 2;
  int listSize = SFIX_estimateMessageSize(size * PROPERTY_LENGTH);
  char list[listSize];
  memset(list, 0, listSize);
  for (int i = 0; i < size; i++) {
    SFIX_Pair orders[PROPERTY_LENGTH] = {0};
    createOrder(&orders[0], "10", "1.25");
    SFIX_compose(list, 'O', orders, PROPERTY_LENGTH);
  }
  printf("\n=== list message \n%s\n",list);

  // 🧰 parse list message
  // =========================================================================="
  SFIX_KeyValue *n = calloc(0,sizeof(SFIX_KeyValue) * 256 * size);
  SFIX_parse(list, n);
  printOrderList(n, size);
  free(n);
}
