import ctypes

lib = ctypes.CDLL("/usr/local/lib/libsfix.so")

def SFIX_MSG_SIZE(pairsLength) -> int:
    return (pairsLength + 3) * (SFIX_MAX_VALUE_LEN + SFIX_MAX_TAG_LEN + 2) + 2

SFIX_MAX_TAG_LEN = 16
SFIX_MAX_VALUE_LEN = 64
SFIX_TAGSNUM = 256


class SFIX_Pair(ctypes.Structure):
    _fields_ = [("key", ctypes.c_int), ("value", ctypes.c_char * 64)]


class SFIX_KeyValue(ctypes.Structure):
    _fields_ = [
        ("tag", ctypes.c_char * SFIX_MAX_TAG_LEN),
        ("value", ctypes.c_char * SFIX_MAX_VALUE_LEN),
    ]
