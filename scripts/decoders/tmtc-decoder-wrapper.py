import ctypes
import sys
import time
import platform
from datetime import datetime


class TMtcPoint(ctypes.Structure):
    _fields_ = [
        ("key", ctypes.c_uint8),
        ("length", ctypes.c_uint8),
        ("time_offset", ctypes.c_uint32),
        ("values", ctypes.POINTER(ctypes.c_uint64))
    ]

class TMtcObject(ctypes.Structure):
    _fields_ = [
        ("points", ctypes.POINTER(TMtcPoint)),
        ("size", ctypes.c_uint64),
        ("_file_length", ctypes.c_uint64),
        ("_allocation_size", ctypes.c_uint64),
        ("is_collapsable", ctypes.c_char)
    ]

class TMtcStream(ctypes.Structure):
    _fields_ = [
        ("object", ctypes.POINTER(TMtcObject)),
        ("_next", ctypes.POINTER(TMtcObject)),
        ("index", ctypes.c_uint16),
        ("offset", ctypes.c_uint64),
        ("fp", ctypes.c_void_p),  # FILE* pointer
        ("_chunk_size", ctypes.c_uint16),
        ("_read_buffer", ctypes.c_void_p),  # byte_t* pointer
        ("flipper", ctypes.c_ubyte)
    ]

if platform.system() == "Windows":
    print("No windows support", file=sys.stderr)
    exit(-1)
else:
    lib_path = "./libmtc_decoder.so"

try:
    lib = ctypes.cdll.LoadLibrary(lib_path)
except OSError as e:
    raise ImportError(f"Failed to load TMtc decoder: {e}")

lib.createTMtcObject.argtypes = [ctypes.POINTER(TMtcObject)]
lib.createTMtcObject.restype = None

lib.destroyTMtcObject.argtypes = [ctypes.POINTER(TMtcObject)]
lib.destroyTMtcObject.restype = None

lib.queryTDecodeProgress.argtypes = [ctypes.POINTER(TMtcObject)]
lib.queryTDecodeProgress.restype = ctypes.c_uint8

lib.decode_tmtc.argtypes = [ctypes.c_char_p, ctypes.POINTER(TMtcObject)]
lib.decode_tmtc.restype = None

lib.createTMtcStream.argtypes = [ctypes.POINTER(TMtcStream)]
lib.createTMtcStream.restype = None

lib.stream_decode_tmtc.argtypes = [ctypes.c_char_p, ctypes.POINTER(TMtcStream), ctypes.c_char]
lib.stream_decode_tmtc.restype = None

lib.stream_tmtc_next.argtypes = [ctypes.POINTER(TMtcStream)]
lib.stream_tmtc_next.restype = ctypes.POINTER(TMtcObject)

lib.stream_tmtc_destroy.argtypes = [ctypes.POINTER(TMtcStream)]
lib.stream_tmtc_destroy.restype = None



if __name__ == "__main__":
    obj = TMtcObject()
    lib.createTMtcObject(ctypes.byref(obj))
    c_filepath = ctypes.c_char_p("/home/duncan/Desktop/memory_tracker.tmtc".encode('utf-8'))
    # s = datetime.now()
    lib.decode_tmtc(c_filepath, ctypes.byref(obj))
    # e = datetime.now()
    # print((e-s).total_seconds())
