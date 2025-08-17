#ifndef ORDER_SFIX_H
#define ORDER_SFIX_H
#include "Uuid.SFIX.h"
#include "Client.SFIX.h"
#include <time.h>

#define T SFIX_Order
#define SFIX_ORDER_PROPERTY_LEN 10
typedef struct T T;

typedef enum {
  SFIX_ORDER_STATUS_CANCELLED,
  SFIX_ORDER_STATUS_REJECTED,
  SFIX_ORDER_STATUS_PENDING,
  SFIX_ORDER_STATUS_EXECUTED,
  $SFIX_ORDER_STATUS_LAST
} SFIX_OrderStatus;

typedef enum {
  SFIX_ORDER_SIDE_NONE,
  SFIX_ORDER_SIDE_BUY,
  SFIX_ORDER_SIDE_SELL,
  $SFIX_ORDER_SIDE_LAST
} SFIX_OrderSide;

typedef enum {
  SFIX_ORDER_TYPE_MARKET,
  SFIX_ORDER_TYPE_LIMIT,
  SFIX_ORDER_TYPE_STOP_LIMIT,
  $SFIX_ORDER_TYPE_LAST
} SFIX_OrderType;

struct T {
  time_t timestamp;
  char candleDate[11];
  char candleTime[9];
  SFIX_Uuid id;
  SFIX_OrderType type;
  SFIX_OrderStatus status;
  SFIX_OrderSide side;
  double size;
  double price;
  char symbol[8];
  int (*toSfix)(T *self, char out[SFIX_MSG_SIZE(SFIX_ORDER_PROPERTY_LEN)]);
  void (*print)(T *self, bool showLabels);
};

/**
 * Order constructor
 *   @param pairs        An array of tag/value pairs
 *   @return             SFIX_Candle
 */
T SFIX_order_constructor(SFIX_KeyValue pairs[SFIX_TAGSNUM]);

/**
 * Validate the integrity of an array of order tag/value pairs
 *   @param pairs        An array of tag/value pairs
 *   @return             bool
 */
bool SFIX_order_validate(SFIX_KeyValue pairs[SFIX_TAGSNUM]);

#undef T
#endif
