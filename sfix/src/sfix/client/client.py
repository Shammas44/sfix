import ctypes
from ctypes import POINTER, c_char_p, c_int, c_char, Array, c_bool
from sfix.ccore.lib import (
    lib,
    SFIX_MAX_TAG_LEN,
    SFIX_MAX_VALUE_LEN,
    SFIX_Pair,
    SFIX_KeyValue,
    SFIX_MSG_SIZE,
)

# [====] CLIENT.H |

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
lib.SFIX_print.argtypes = [c_char_p, c_bool]
lib.SFIX_print.restype = None

# int SFIX_acknowledge(char out[248]);
lib.SFIX_acknowledge.argtypes = [c_char * 248]
lib.SFIX_acknowledge.restype = c_int

# int SFIX_strategy(const char name[64],char out[330]);
lib.SFIX_strategy.argtypes = [c_char *64, c_char * 330]
lib.SFIX_strategy.restype = c_int

# int SFIX_error(int status,const char description[64],char out[412]);
lib.SFIX_error.argtypes = [c_int, c_char *64, c_char * 412]
lib.SFIX_error.restype = c_int

# int SFIX_unknown(char out[248]);
lib.SFIX_unknown.argtypes = [c_char * 248]
lib.SFIX_unknown.restype = c_int


def sfix_compose(buffer: Array[c_char], msg_type: str, pairs: Array[SFIX_Pair]) -> str:
    ##region
    pairs_array = (SFIX_Pair * len(pairs))(*pairs)
    length = len(pairs)
    type = msg_type.encode("utf-8")
    lib.SFIX_compose(buffer, type, pairs_array, length)
    return buffer.value.decode("utf-8")
    ##endregion


def sfix_createBuffer(pairsCount: int) -> Array[c_char]:
    ##region
    size: int = lib.SFIX_estimateMessageSize(pairsCount)
    return ctypes.create_string_buffer(size)
    ##endregion


def sfix_parse(message: str) -> Array[SFIX_KeyValue]:
    ##region
    message_bytes = message.encode("utf-8")
    length = lib.SFIX_listLength(message_bytes)
    pairsCount = length * 256
    keys_array: Array[SFIX_KeyValue] = (SFIX_KeyValue * pairsCount)()
    keys_ptr = ctypes.cast(keys_array, ctypes.POINTER(SFIX_KeyValue))
    result = lib.SFIX_parse(message_bytes, keys_ptr)
    return keys_array
    ##endregion


def sfix_listLength(msg: str):
    ##region
    return lib.SFIX_listLength(msg.encode("utf-8"))
    ##endregion


def sfix_acknowledge() -> str:
    ##region
    buffer = (ctypes.c_char * 248)()
    lib.SFIX_acknowledge(buffer)
    return buffer.value.decode("utf-8")
    ##endregion

def sfix_strategy(name:str) -> str:
    ##region
    buffer = (ctypes.c_char * 330)()
    name_bytes = name.encode('utf-8')
    name_buffer = (ctypes.c_char * 64)(*name_bytes)
    lib.SFIX_strategy(name_buffer,buffer)
    return buffer.value.decode("utf-8")
    ##endregion

def sfix_error(status:int, description:str) -> str:
    ##region
    buffer = (ctypes.c_char * 412)()
    description_bytes = description.encode('utf-8')
    description_buffer = (ctypes.c_char * 64)(*description_bytes)
    lib.SFIX_error(status,description_buffer,buffer)
    return buffer.value.decode("utf-8")
    ##endregion


def sfix_unknown() -> str:
    ##region
    buffer = (ctypes.c_char * 248)()
    lib.SFIX_unknown(buffer)
    return buffer.value.decode("utf-8")
    ##endregion


def sfix_print(message: str, showLabel=False) -> None:
    ##region
    lib.SFIX_print(message.encode("utf-8"), showLabel)
    ##endregion
