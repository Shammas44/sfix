import ctypes
from sfix.ccore.lib import lib, SFIX_TAGSNUM, SFIX_KeyValue, SFIX_MSG_SIZE


class SFIX_Candle(ctypes.Structure):
    pass


TO_SFIX_FUNC = ctypes.CFUNCTYPE(
    ctypes.c_int,
    ctypes.POINTER(SFIX_Candle),ctypes.POINTER(ctypes.c_char * SFIX_MSG_SIZE(13)),
)

PRINT_FUNC = ctypes.CFUNCTYPE(
    None, ctypes.POINTER(SFIX_Candle), ctypes.c_bool
)

SFIX_Candle._fields_ = [
    ("timestamp", ctypes.c_long),
    ("date", ctypes.c_char * 11),
    ("time", ctypes.c_char * 9),
    ("open", ctypes.c_double),
    ("high", ctypes.c_double),
    ("low", ctypes.c_double),
    ("close", ctypes.c_double),
    ("upVolume", ctypes.c_int),
    ("downVolume", ctypes.c_int),
    ("totalVolume", ctypes.c_int),
    ("upTicks", ctypes.c_int),
    ("downTicks", ctypes.c_int),
    ("totalTicks", ctypes.c_int),
    ("toSfix", TO_SFIX_FUNC),
    ("print", PRINT_FUNC),
]


lib.SFIX_candle_constructor.argtypes = [SFIX_KeyValue * SFIX_TAGSNUM]
lib.SFIX_candle_constructor.restype = SFIX_Candle

lib.SFIX_candle_validate.argtypes = [SFIX_KeyValue * SFIX_TAGSNUM]
lib.SFIX_candle_validate.restype = ctypes.c_bool

def sfix_candle_constructor(array: ctypes.Array[SFIX_KeyValue]) -> SFIX_Candle:
    ##region
    return lib.SFIX_candle_constructor(array)
    ##endregion

def sfix_candle_validate(array: ctypes.Array[SFIX_KeyValue]) -> ctypes.c_bool:
    ##region
    return lib.SFIX_candle_validate(array)
    ##endregion
