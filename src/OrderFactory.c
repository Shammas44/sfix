#include "OrderFactory.SFIX.h"
#include <stdio.h>
#include <string.h>
#define T SFIX_OrderFactory

static T *__init(T *self, SFIX_Order *buffer);
static T *__setId(T *self, SFIX_Uuid id);
static T *__setCandleDate(T *self, char date[11]);
static T *__setCandleTime(T *self, char time[9]);
static T *__setType(T *self, SFIX_OrderType type);
static T *__setStatus(T *self, SFIX_OrderStatus status);
static T *__setSide(T *self, SFIX_OrderSide side);
static T *__setSize(T *self, double size);
static T *__setPrice(T *self, double price);
static T *__setSymbol(T *self, char *symbol);
static T *__generateId(T *self);
static T *__build(T *self);

/**
 * @brief Constructor for the order.
 * Initializes the factory and sets up its method pointers.
 * @param defaultCurrency The default currency symbol to use for new orders.
 * @return An initialized order instance.
 */
T SFIX_orderFactory_constructor(char defaultSymbol[8]) {
  // #region
  T factory;
  factory.state = NULL; // Initially, no order buffer is set

  // Assign function pointers
  factory.init = __init;
  factory.generateId = __generateId;
  factory.setId = __setId;
  factory.setCandleDate = __setCandleDate;
  factory.setCandleTime = __setCandleTime;
  factory.setType = __setType;
  factory.setStatus = __setStatus;
  factory.setSide = __setSide;
  factory.setSize = __setSize;
  factory.setPrice = __setPrice;
  factory.setSymbol = __setSymbol;
  factory.build = __build;

  // Set the default currency
  strncpy(factory.__defaultSymbol, defaultSymbol,
          sizeof(factory.__defaultSymbol) - 1);
  factory.__defaultSymbol[sizeof(factory.__defaultSymbol) - 1] = '\0';

  // Initialize the internal pairs array to zero
  memset(factory.__internal_pairs, 0, sizeof(factory.__internal_pairs));

  return factory;
  // #endregion
}

/**
 * @brief Initializes the order and prepares the internal key-value pairs array.
 * This method sets the internal state pointer to the target SFIX_Order buffer
 * and initializes the internal SFIX_KeyValue array with default values.
 * @param self Pointer to the order instance.
 * @param buffer Pointer to the SFIX_Order structure to be populated by the
 * build method.
 * @return Pointer to the order instance for chaining.
 */
static T *__init(T *self, SFIX_Order *buffer) {
  // #region
  if (self == NULL || buffer == NULL) {
    fprintf(stderr, "Error: __init received NULL self or buffer.\n");
    return NULL;
  }
  self->state = buffer;
  // Clear the internal pairs array
  memset(self->__internal_pairs, 0, sizeof(self->__internal_pairs));

  // Set initial values for timestamp and default currency in the internal pairs
  snprintf(self->__internal_pairs[SFIX_Tag_Timestamp].value, SFIX_MAX_VALUE_LEN,
           "%ld", (long)time(NULL));

  snprintf(self->__internal_pairs[SFIX_Tag_Timestamp].tag, SFIX_MAX_TAG_LEN,
           "%s", self->__internal_pairs[SFIX_Tag_Timestamp].tag);

  strncpy(self->__internal_pairs[SFIX_Tag_Symbol].value, self->__defaultSymbol,
          SFIX_MAX_VALUE_LEN - 1);
  self->__internal_pairs[SFIX_Tag_Symbol].value[SFIX_MAX_VALUE_LEN - 1] = '\0';

  snprintf(self->__internal_pairs[SFIX_Tag_Symbol].tag, SFIX_MAX_TAG_LEN, "%d",
           SFIX_Tag_Symbol);
  return __generateId(self);
  // #endregion
}

/**
 * @brief Generates a new UUID string and stores it in the internal pairs array.
 * Requires libuuid development package (e.g., `libuuid-dev` on Debian/Ubuntu).
 * @param self Pointer to the order instance.
 * @return Pointer to the order instance for chaining.
 */
static T *__generateId(T *self) {
  // #region
  if (self == NULL) {
    fprintf(stderr, "Error: __generateId: NULL self.\n");
    return NULL;
  }
  __setId(self, SFIX_uuid_constructor());
  return self;
  // #endregion
}

/**
 * @brief Sets the ID string in the internal pairs array.
 * @param self Pointer to the order instance.
 * @param id The SFIX_Uuid (string) to set.
 * @return Pointer to the order instance for chaining.
 */
static T *__setId(T *self, SFIX_Uuid id) {
  // #region
  if (self == NULL) {
    fprintf(stderr, "Error: __setId: NULL self.\n");
    return NULL;
  }

  char value[SFIX_UUID_LENGTH] = {0};
  id.get(id, value);
  strncpy(self->__internal_pairs[SFIX_Tag_Client_order_id].value, value,
          SFIX_MAX_VALUE_LEN - 1);
  self->__internal_pairs[SFIX_Tag_Client_order_id]
      .value[SFIX_MAX_VALUE_LEN - 1] = '\0';

  snprintf(self->__internal_pairs[SFIX_Tag_Client_order_id].tag,
           SFIX_MAX_TAG_LEN, "%d", SFIX_Tag_Client_order_id);
  return self;
  // #endregion
}

/**
 * @brief Sets the order type as a string in the internal pairs array.
 * @param self Pointer to the order instance.
 * @param type The SFIX_OrderType enum value.
 * @return Pointer to the order instance for chaining.
 */
static T *__setType(T *self, SFIX_OrderType type) {
  // #region
  if (self == NULL) {
    fprintf(stderr, "Error: __setType: NULL self.\n");
    return NULL;
  }
  snprintf(self->__internal_pairs[SFIX_Tag_Order_type].value,
           SFIX_MAX_VALUE_LEN, "%d", type);
  snprintf(self->__internal_pairs[SFIX_Tag_Order_type].tag, SFIX_MAX_TAG_LEN,
           "%d", SFIX_Tag_Order_type);
  return self;
  // #endregion
}

/**
 * @brief Sets the order status as a string in the internal pairs array.
 * @param self Pointer to the order instance.
 * @param status The SFIX_OrderStatus enum value.
 * @return Pointer to the order instance for chaining.
 */
static T *__setStatus(T *self, SFIX_OrderStatus status) {
  // #region
  if (self == NULL) {
    fprintf(stderr, "Error: __setStatus: NULL self.\n");
    return NULL;
  }
  snprintf(self->__internal_pairs[SFIX_Tag_Order_status].value,
           SFIX_MAX_VALUE_LEN, "%d", status);
  snprintf(self->__internal_pairs[SFIX_Tag_Order_status].tag, SFIX_MAX_TAG_LEN,
           "%d", SFIX_Tag_Order_status);
  return self;
  // #endregion
}

/**
 * @brief Sets the order side as a string in the internal pairs array.
 * @param self Pointer to the order instance.
 * @param side The SFIX_OrderSide enum value.
 * @return Pointer to the order instance for chaining.
 */
static T *__setSide(T *self, SFIX_OrderSide side) {
  // #region
  if (self == NULL) {
    fprintf(stderr, "Error: __setSide: NULL self.\n");
    return NULL;
  }
  snprintf(self->__internal_pairs[SFIX_Tag_Side].value, SFIX_MAX_VALUE_LEN,
           "%d", side);
  snprintf(self->__internal_pairs[SFIX_Tag_Side].tag, SFIX_MAX_TAG_LEN, "%d",
           SFIX_Tag_Side);
  return self;
  // #endregion
}

/**
 * @brief Sets the order size (quantity) as a string in the internal pairs
 * array.
 * @param self Pointer to the order instance.
 * @param size The double value for quantity.
 * @return Pointer to the order instance for chaining.
 */
static T *__setSize(T *self, double size) {
  // #region
  if (self == NULL) {
    fprintf(stderr, "Error: __setSize: NULL self.\n");
    return NULL;
  }
  snprintf(self->__internal_pairs[SFIX_Tag_Quantity].value, SFIX_MAX_VALUE_LEN,
           "%.10f", size);
  snprintf(self->__internal_pairs[SFIX_Tag_Quantity].tag, SFIX_MAX_TAG_LEN,
           "%d", SFIX_Tag_Quantity);
  return self;
  // #endregion
}

/**
 * @brief Sets the order price as a string in the internal pairs array.
 * @param self Pointer to the order instance.
 * @param price The double value for price.
 * @return Pointer to the order instance for chaining.
 */
static T *__setPrice(T *self, double price) {
  // #region
  if (self == NULL) {
    fprintf(stderr, "Error: __setPrice: NULL self.\n");
    return NULL;
  }
  snprintf(self->__internal_pairs[SFIX_Tag_Price].value, SFIX_MAX_VALUE_LEN,
           "%.10f", price);
  snprintf(self->__internal_pairs[SFIX_Tag_Price].tag, SFIX_MAX_TAG_LEN, "%d",
           SFIX_Tag_Price);
  return self;
  // #endregion
}

/**
 * @brief Sets the candleDate as a string in the internal pairs array.
 * @param self Pointer to the order instance.
 * @param date string formatted like "yyyy-mm-dd"
 * @return Pointer to the order instance for chaining.
 */
static T *__setCandleDate(T *self, char date[11]) {
  /*#region*/
  if (self == NULL) {
    fprintf(stderr, "Error: __setCandleDate: NULL self.\n");
    return NULL;
  }
  strncpy(self->__internal_pairs[SFIX_Tag_Date].value, date,
          SFIX_MAX_VALUE_LEN - 1);
  self->__internal_pairs[SFIX_Tag_Date].value[SFIX_MAX_VALUE_LEN - 1] = '\0';

  snprintf(self->__internal_pairs[SFIX_Tag_Date].tag, SFIX_MAX_TAG_LEN, "%d",
           SFIX_Tag_Date);
  return self;
  /*#endregion*/
}

/**
 * @brief Sets the candleTime as a string in the internal pairs array.
 * @param self Pointer to the order instance.
 * @param time string formatted like "hh:mm:ss"
 * @return Pointer to the order instance for chaining.
 */
static T *__setCandleTime(T *self, char time[9]) {
  /*#region*/
  if (self == NULL) {
    fprintf(stderr, "Error: __setCandleTime: NULL self.\n");
    return NULL;
  }
  strncpy(self->__internal_pairs[SFIX_Tag_Time].value, time,
          SFIX_MAX_VALUE_LEN - 1);
  self->__internal_pairs[SFIX_Tag_Time].value[SFIX_MAX_VALUE_LEN - 1] = '\0';

  snprintf(self->__internal_pairs[SFIX_Tag_Time].tag, SFIX_MAX_TAG_LEN, "%d",
           SFIX_Tag_Time);
  return self;
  /*#endregion*/
}

/**
 * @brief Sets the symbol string in the internal pairs array.
 * @param self Pointer to the order instance.
 * @param symbol The 8-character string for symbol (e.g., "BTCUSD").
 * @return Pointer to the order instance for chaining.
 */
static T *__setSymbol(T *self, char symbol[8]) {
  // #region
  if (self == NULL) {
    fprintf(stderr, "Error: __setSymbol: NULL self.\n");
    return NULL;
  }
  strncpy(self->__internal_pairs[SFIX_Tag_Symbol].value, symbol,
          SFIX_MAX_VALUE_LEN - 1);
  self->__internal_pairs[SFIX_Tag_Symbol].value[SFIX_MAX_VALUE_LEN - 1] = '\0';

  snprintf(self->__internal_pairs[SFIX_Tag_Symbol].tag, SFIX_MAX_TAG_LEN, "%d",
           SFIX_Tag_Symbol);
  return self;
  // #endregion
}

/**
 * @brief Builds the SFIX_Order using the SFIX_order_constructor and copies it
 * into the buffer provided during initialization.
 * This should be the final call in the builder chain.
 * @param self Pointer to the order instance.
 * @return Pointer to the order instance for chaining (though typically not
 * chained further).
 */
static T *__build(T *self) {
  // #region
  if (self == NULL || self->state == NULL) {
    fprintf(stderr,
            "Error: __build: Builder not initialized or no buffer provided.\n");
    return NULL;
  }

  // Call the original SFIX_order_constructor with the internally prepared pairs
  SFIX_Order constructed_order = SFIX_order_constructor(self->__internal_pairs);

  // Copy the constructed order into the buffer provided by the user
  memcpy(self->state, &constructed_order, sizeof(SFIX_Order));

  return self;
  // #endregion
}
