import ctypes
from ctypes import Structure, POINTER, c_char_p, c_int, c_char ,Array, _Pointer

SFIX_MAX_TAG_LEN = 16
SFIX_MAX_VALUE_LEN = 64
SFIX_TAGSNUM = 256

def SFIX_MSG_SIZE(pairsLength)-> int:
    return ((pairsLength + 3) * (SFIX_MAX_VALUE_LEN + SFIX_MAX_TAG_LEN + 2) + 2)

lib = ctypes.CDLL("/usr/local/lib/libsfix.so")

class SFIX_Pair(Structure):
    _fields_ = [("key", c_int), ("value", c_char_p)]


class SFIX_KeyValue(Structure):
    _fields_ = [
        ("tag", c_char * SFIX_MAX_TAG_LEN),
        ("value", c_char * SFIX_MAX_VALUE_LEN),
    ]

# int SFIX_compose(char out[], char type, SFIX_Pair pairs[], int pairsLength);
lib.SFIX_compose.argtypes = [c_char_p, c_char, POINTER(SFIX_Pair), c_int]
lib.SFIX_compose.restype = c_int

# int SFIX_estimateMessageSize(int pairsLength);
lib.SFIX_estimateMessageSize.argtypes = [c_int]
lib.SFIX_estimateMessageSize.restype = c_int

# int SFIX_listLength(char *message);
lib.SFIX_listLength.argtypes = [c_char_p]
lib.SFIX_listLength.restype = c_int

# int SFIX_parse(char *message, SFIX_KeyValue *out);
lib.SFIX_parse.argtypes = [c_char_p, POINTER(SFIX_KeyValue)]
lib.SFIX_parse.restype = c_int

# void SFIX_print(char *message);
lib.SFIX_print.argtypes = [c_char_p]
lib.SFIX_print.restype = None

# int SFIX_acknowledge(char out[248]);
lib.SFIX_acknowledge.argtypes = [c_char * 248]
lib.SFIX_acknowledge.restype = c_int

# int SFIX_unknown(char out[248]);
lib.SFIX_unknown.argtypes = [c_char * 248]
lib.SFIX_unknown.restype = c_int


def sfix_compose(buffer:Array[c_char],msg_type:str, pairs:Array[SFIX_Pair])-> str:
    ##region
    pairs_array = (SFIX_Pair * len(pairs))(*pairs)
    length = len(pairs)
    type = msg_type.encode('utf-8')
    lib.SFIX_compose(buffer,type, pairs_array, length)
    return buffer.value.decode("utf-8")
    ##endregion

def SFIX_createBuffer(pairsCount:int)->Array[c_char]:
    ##region
    size:int = lib.SFIX_estimateMessageSize(pairsCount)
    return ctypes.create_string_buffer(size)
    ##endregion

def sfix_parse(message:str)-> Array[SFIX_KeyValue]:
    ##region
    message_bytes = message.encode("utf-8")
    length = lib.SFIX_listLength(message_bytes);
    pairsCount = length * 256
    keys_array:Array[SFIX_KeyValue] = (SFIX_KeyValue * pairsCount)()
    keys_ptr = ctypes.cast(keys_array, ctypes.POINTER(SFIX_KeyValue))
    result = lib.SFIX_parse(message_bytes, keys_ptr)
    return keys_array
    ##endregion

def sfix_listLength(msg:str):
    ##region
    return lib.SFIX_listLength(msg.encode("utf-8"))
    ##endregion

def sfix_acknowledge()-> str:
    ##region
    buffer = (ctypes.c_char * 248)()
    lib.SFIX_acknowledge(buffer)
    return buffer.value.decode("utf-8")
    ##endregion

def sfix_unknown()-> str:
    ##region
    buffer = (ctypes.c_char * 248)()
    lib.SFIX_unknown(buffer)
    return buffer.value.decode("utf-8")
    ##endregion

def sfix_print(message: str)-> None:
    ##region
    lib.SFIX_print(message.encode("utf-8"))
    ##endregion
