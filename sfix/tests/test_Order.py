from sfix.client.client import *
from sfix.client.dictionary import *
from sfix.domain.Order import *
import unittest
import time


def to_string(num: int):
    ##region
    value = str(num)
    return value.encode("utf-8")
    ##endregion


def create_pairs():
    ##region
    pairs = (SFIX_KeyValue * 256)()
    pairs[Tag.TIMESTAMP.value] = SFIX_KeyValue( to_string(Tag.TIMESTAMP.value), to_string(int(time.time())))
    type = SFIX_OrderType.MARKET
    pairs[Tag.ORDER_TYPE.value] = SFIX_KeyValue( to_string(Tag.ORDER_TYPE.value), to_string(type))
    status = SFIX_OrderStatus.CANCELLED
    pairs[Tag.ORDER_STATUS.value] = SFIX_KeyValue( to_string(Tag.ORDER_STATUS.value), to_string(status))
    side = SFIX_OrderSide.BUY
    pairs[Tag.SIDE.value] = SFIX_KeyValue(to_string(Tag.SIDE.value), to_string(side))
    pairs[Tag.QUANTITY.value] = SFIX_KeyValue(to_string(Tag.QUANTITY.value), to_string(2))
    pairs[Tag.PRICE.value] = SFIX_KeyValue(to_string(Tag.PRICE.value), to_string(2))
    pairs[Tag.SYMBOL.value] = SFIX_KeyValue(to_string(Tag.SYMBOL.value), "BTC".encode("utf-8"))
    pairs[Tag.DATE.value] = SFIX_KeyValue(to_string(Tag.DATE.value), "2024-01-01".encode("utf-8"))
    pairs[Tag.TIME.value] = SFIX_KeyValue(to_string(Tag.TIME.value), "08:00:00".encode("utf-8"))
    return pairs
    ##endregion


def expectOrder(order: SFIX_Order, t: unittest.TestCase):
    ##region
    t.assertEqual(order.type.value, SFIX_OrderType.MARKET)
    t.assertEqual(order.status.value, SFIX_OrderStatus.CANCELLED)
    t.assertEqual(order.side.value, SFIX_OrderSide.BUY)
    t.assertEqual(order.size, 2)
    t.assertEqual(order.price, 2)
    t.assertEqual(order.symbol.decode("utf-8"), "BTC")
    t.assertEqual(order.candleDate.decode("utf-8"), "2024-01-01")
    t.assertEqual(order.candleTime.decode("utf-8"), "08:00:00")
    ##endregion


class SFIX_test_order(unittest.TestCase):

    def test_order_constructor(self):
        ##region
        pairs = create_pairs()
        order = sfix_order_constructor(pairs)
        expectOrder(order, self)
        ##endregion

    def test_to_sfix(self):
        ##region
        pairs = create_pairs()
        order = sfix_order_constructor(pairs)
        buffer = ctypes.create_string_buffer(SFIX_MSG_SIZE(SFIX_ORDER_PROPERTY_LEN))
        order.toSfix(order, buffer)
        array = sfix_parse(buffer.value.decode("utf-8"))
        newOrder = sfix_order_constructor(array)
        expectOrder(newOrder, self)
        ##endregion


if __name__ == "__main__":
    unittest.main()
