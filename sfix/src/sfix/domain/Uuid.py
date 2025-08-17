import ctypes
from ctypes import Structure, POINTER, CFUNCTYPE, c_char, c_bool, c_size_t

# Constants
SFIX_UUID_LENGTH = 32

class SFIX_Uuid(ctypes.Structure):
    pass

# Function pointer types
SFIX_GetFunc = CFUNCTYPE(None, SFIX_Uuid, ctypes.c_char * SFIX_UUID_LENGTH)
SFIX_CompareFunc = CFUNCTYPE(c_bool, SFIX_Uuid, SFIX_Uuid)

# Struct definition
SFIX_Uuid._fields_ = [
    ("get", SFIX_GetFunc),
    ("compare", SFIX_CompareFunc),
    ("__value", c_char * SFIX_UUID_LENGTH),
]

# lib.SFIX_uuid_constructor.restype = SFIX_Uuid

# lib.SFIX_uuid_by_string_constructor.argtypes = [ctypes.c_char * SFIX_UUID_LENGTH]
# lib.SFIX_uuid_by_string_constructor.restype = SFIX_Uuid

# lib.SFIX_uuid_by_custom_length_string.argtypes = [ctypes.POINTER(c_char), c_size_t, c_size_t]
# lib.SFIX_uuid_by_custom_length_string.restype = None
