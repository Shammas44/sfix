#ifndef ORDERFACTORY_SFIX_H
#define ORDERFACTORY_SFIX_H
#include "Order.SFIX.h"

#define T SFIX_OrderFactory

typedef struct T T;

struct T {
  SFIX_Order *state; // Pointer to the SFIX_Order buffer to be filled
  T *(*init)(T *self, SFIX_Order *buffer);
  T *(*generateId)(T *self);
  T *(*setCandleDate)(T *self, char date[11]);
  T *(*setCandleTime)(T *self, char time[9]);
  T *(*setId)(T *self, SFIX_Uuid id);
  T *(*setType)(T *self, SFIX_OrderType type);
  T *(*setStatus)(T *self, SFIX_OrderStatus status);
  T *(*setSide)(T *self, SFIX_OrderSide side);
  T *(*setSize)(T *self, double size);
  T *(*setPrice)(T *self, double price);
  T *(*setSymbol)(T *self, char symbol[8]);
  T *(*build)(T *self); // New build method
  SFIX_KeyValue
      __internal_pairs[SFIX_TAGSNUM]; // Internal storage for key-value pairs
  char __defaultSymbol[8];
};

T SFIX_orderFactory_constructor(char symbol[8]);

#undef T
#endif
