#include <sfix/sfix.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROPERTY_LEN 8
void createOrder(SFIX_Pair input[PROPERTY_LEN], char qty[64], char price[64]) {
  /*#region*/
  int i = 0;
  input[i++] = (SFIX_Pair){SFIX_Tag_Timestamp, "100000"};
  input[i++] = (SFIX_Pair){SFIX_Tag_Client_order_id, "id"};
  input[i++] = (SFIX_Pair){SFIX_Tag_Order_type, "market"};
  input[i++] = (SFIX_Pair){SFIX_Tag_Order_status, "pending"};
  input[i++] = (SFIX_Pair){SFIX_Tag_Side, "buy"};
  strcpy(input[i].value, qty);
  input[i++].key = SFIX_Tag_Quantity;
  strcpy(input[i].value, price);
  input[i++].key = SFIX_Tag_Price;
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
  SFIX_print(unknown, false);

  // 🧰 Compose list message
  // =========================================================================="
  int size = 2;
  int listSize = SFIX_estimateMessageSize(size * PROPERTY_LEN);
  char list[listSize];
  memset(list, 0, listSize);
  for (int i = 0; i < size; i++) {
    SFIX_Pair orders[PROPERTY_LEN] = {0};
    createOrder(&orders[0], "10", "1.25");
    SFIX_compose(list, SFIX_Symbol_Order, orders, PROPERTY_LEN);
  }
  printf("\n=== list message \n%s\n", list);

  // 🧰 parse list message
  // =========================================================================="
  SFIX_KeyValue *n = calloc(0, sizeof(SFIX_KeyValue) * 256 * size);
  SFIX_parse(list, n);
  printOrderList(n, size);
  free(n);
}
