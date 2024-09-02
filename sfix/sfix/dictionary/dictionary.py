from enum import Enum


class Strategy(Enum):
    NONE = 0
    STRATEGY_TEST = 1
    STRATEGY_LASTKISS = 2


class OrderStatus(Enum):
    CANCELLED = 0
    REJECTED = 1
    PENDING = 2
    EXECUTED = 3


class OrderSide(Enum):
    BUY = 0
    SELL = 1


class OrderType(Enum):
    MARKET = 0
    LIMIT = 1
    STOP_LIMIT = 2


class Tag(Enum):
    PROTOCOL_VERSION = 8
    BODY_LENGTH = 9
    CHECKSUM = 10
    CLIENT_ORDER_ID = 11
    TIMESTAMP = 12
    MESSAGE_TYPE = 35
    QUANTITY = 38
    PRICE = 44
    ORDER_TYPE = 45
    ORDER_STATUS = 46
    SENDER_COMP_ID = 49
    SIDE = 54
    SYMBOL = 55
    TARGET_COMP_ID = 56
    TIME_IN_FORCE = 59
    TRANSACTION_TIME = 60
    DATE = 61
    TIME = 62
    OPEN = 63
    HIGH = 64
    LOW = 65
    CLOSE = 66
    UP_VOLUME = 67
    DOWN_VOLUME = 69
    TOTAL_VOLUME = 70
    UP_TICKS = 71
    DOWN_TICKS = 72
    TOTAL_TICKS = 73
    CAPITAL = 74
    MARKET_POSITION = 75
    EQUITY = 76
    LAST_PRICE = 77
    STRATEGY = 78
    SPREAD = 79
    NUMBER_OF_TRADES = 80
    BROKER_COMMISION_DPM = 81
