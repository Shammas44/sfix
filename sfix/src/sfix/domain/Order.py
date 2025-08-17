import ctypes
import ctypes
from sfix.domain.Uuid import SFIX_Uuid
from sfix.ccore.lib import lib, SFIX_KeyValue, SFIX_TAGSNUM, SFIX_MSG_SIZE

SFIX_ORDER_PROPERTY_LEN = 10


class SFIX_OrderStatus(ctypes.c_int):
    CANCELLED = 0
    REJECTED = 1
    PENDING = 2
    EXECUTED = 3
    LAST = 4


class SFIX_OrderSide(ctypes.c_int):
    NONE = 0
    BUY = 1
    SELL = 2
    LAST = 3


class SFIX_OrderType(ctypes.c_int):
    MARKET = 0
    LIMIT = 1
    STOP_LIMIT = 2
    LAST = 3


class SFIX_Order(ctypes.Structure):
    pass


TO_SFIX_FUNC = ctypes.CFUNCTYPE(
    ctypes.c_int,
    ctypes.POINTER(SFIX_Order),
    ctypes.POINTER(ctypes.c_char * SFIX_MSG_SIZE(SFIX_ORDER_PROPERTY_LEN)),
)

PRINT_FUNC = ctypes.CFUNCTYPE(None, ctypes.POINTER(SFIX_Order), ctypes.c_bool)


SFIX_Order._fields_ = [
    ("timestamp", ctypes.c_long),
    ("candleDate", ctypes.c_char * 11),
    ("candleTime", ctypes.c_char * 9),
    ("id", SFIX_Uuid),
    ("type", SFIX_OrderType),
    ("status", SFIX_OrderStatus),
    ("side", SFIX_OrderSide),
    ("size", ctypes.c_double),
    ("price", ctypes.c_double),
    ("symbol", ctypes.c_char * 8),
    ("toSfix", TO_SFIX_FUNC),
    ("print", PRINT_FUNC),
]

lib.SFIX_order_constructor.argtypes = [SFIX_KeyValue * SFIX_TAGSNUM]
lib.SFIX_order_constructor.restype = SFIX_Order

lib.SFIX_order_validate.argtypes = [SFIX_KeyValue * SFIX_TAGSNUM]
lib.SFIX_order_validate.restype = ctypes.c_bool


def sfix_order_constructor(array: ctypes.Array[SFIX_KeyValue]) -> SFIX_Order:
    ##region
    return lib.SFIX_order_constructor(array)
    ##endregion


def sfix_order_validate(array: ctypes.Array[SFIX_KeyValue]) -> SFIX_Order:
    ##region
    return lib.SFIX_order_validate(array)
    ##endregion
