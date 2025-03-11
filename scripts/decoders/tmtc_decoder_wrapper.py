import ctypes
import sys
import platform


class TMtcMeta(type):
    def __init__(cls, name, bases, class_dict):
        super().__init__(name, bases, class_dict)

        if platform.system() == "Windows":
            print("No windows support", file=sys.stderr)
            exit(-1)
        else:
            lib_path = "./libmtc_decoder.so"

        try:
            cls.__lib = ctypes.cdll.LoadLibrary(lib_path)
        except OSError as e:
            raise ImportError(f"Failed to load TMtc decoder: {e}")

        init_lib = getattr(cls, f"_{cls.__name__}__init_lib", None)
        if init_lib is not None:
            init_lib()
        else:
            print("Failed to initialise decode library", file=sys.stderr)

class _TMtcPoint(ctypes.Structure):
    _fields_ = [
        ("key", ctypes.c_uint8),
        ("length", ctypes.c_uint8),
        ("time_offset", ctypes.c_uint32),
        ("values", ctypes.POINTER(ctypes.c_uint64))
    ]

class _TMtcObject(ctypes.Structure):
    _fields_ = [
        ("points", ctypes.POINTER(_TMtcPoint)),
        ("size", ctypes.c_uint64),
        ("_file_length", ctypes.c_uint64),
        ("_allocation_size", ctypes.c_uint64),
        ("is_collapsable", ctypes.c_char)
    ]

class _TMtcStream(ctypes.Structure):
    _fields_ = [
        ("object", ctypes.POINTER(_TMtcObject)),
        ("_next", ctypes.POINTER(_TMtcObject)),
        ("index", ctypes.c_uint16),
        ("offset", ctypes.c_uint64),
        ("fp", ctypes.c_void_p),  # FILE* pointer
        ("_chunk_size", ctypes.c_uint16),
        ("_read_buffer", ctypes.c_void_p),  # byte_t* pointer
        ("flipper", ctypes.c_ubyte)
    ]


class TMtcPoints:

    def __init__(self, _points, _size):
        self._points = _points
        self.size = _size

    def __iter__(self):
        return iter(self._points)


class TMtcPoint:

    def __init__(self, key, length, time_offset, values):
        self.key = key
        self.length = length
        self.time_offset = time_offset
        self.values = values


class TMtcDecoder(metaclass=TMtcMeta):

    @classmethod
    def __init_lib(cls):
        cls.__lib = cls._TMtcMeta__lib
        cls.__lib.createTMtcObject.argtypes = [ctypes.POINTER(_TMtcObject)]
        cls.__lib.createTMtcObject.restype = None

        cls.__lib.destroyTMtcObject.argtypes = [ctypes.POINTER(_TMtcObject)]
        cls.__lib.destroyTMtcObject.restype = None

        cls.__lib.queryTDecodeProgress.argtypes = [ctypes.POINTER(_TMtcObject)]
        cls.__lib.queryTDecodeProgress.restype = ctypes.c_uint8

        cls.__lib.decode_tmtc.argtypes = [ctypes.c_char_p, ctypes.POINTER(_TMtcObject)]
        cls.__lib.decode_tmtc.restype = None

        cls.__lib.createTMtcStream.argtypes = [ctypes.POINTER(_TMtcStream)]
        cls.__lib.createTMtcStream.restype = None

        cls.__lib.stream_decode_tmtc.argtypes = [ctypes.c_char_p, ctypes.POINTER(_TMtcStream), ctypes.c_char]
        cls.__lib.stream_decode_tmtc.restype = None

        cls.__lib.stream_tmtc_next.argtypes = [ctypes.POINTER(_TMtcStream)]
        cls.__lib.stream_tmtc_next.restype = ctypes.POINTER(_TMtcObject)

        cls.__lib.stream_tmtc_destroy.argtypes = [ctypes.POINTER(_TMtcStream)]
        cls.__lib.stream_tmtc_destroy.restype = None


    def __init__(self):
        self.__obj = _TMtcObject()
        self._lib = self.__class__.__lib
        self._lib.createTMtcObject(ctypes.byref(self.__obj))

    def __del__(self):
        self._lib.destroyTMtcObject(ctypes.byref(self.__obj))


    def __decode_internal(self):
        for i in range(self.__obj.size):
            point = self.__obj.points[i]
            values_array = ctypes.cast(
                point.values,
                ctypes.POINTER(ctypes.c_uint64 * point.length)
            ).contents
            yield TMtcPoint(point.key, point.length, point.time_offset, values_array)

    def decode(self, file: str):
        c_file = ctypes.c_char_p(file.encode('utf-8'))
        self._lib.decode_tmtc(c_file, ctypes.byref(self.__obj))
        return TMtcPoints(self.__decode_internal(), self.__obj.size)

if __name__ == "__main__":
    decoder = TMtcDecoder()
    print("Decoding...")
    points = decoder.decode("/home/duncan/Development/Uni/Thesis/Logs/chrometrackingrepro/memory_tracker.tmtc")
    print("Decoded...\n")
    address_map = {}


