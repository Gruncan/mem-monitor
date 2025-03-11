import json
from pprint import pprint

from tmtc_meta import *

from tmtc_decoder_wrapper import TMtcDecoder



MALLOC = 0x0
CALLOC = 0x1
REALLOC = 0x2
REALLOC_ARRAY = 0x3
FREE = 0x4
NEW = 0x5
NEW_NOTHROW = 0x6
NEW_ARRAY = 0x7
NEW_ARRAY_NOTHROW = 0x8
DELETE = 0x9
DELETE_SIZED = 0xa
DELETE_NOTHROW = 0xb
DELETE_ARRAY = 0xc
DELETE_ARRAY_SIZED = 0xd
DELETE_ARRAY_NOTHROW = 0xe
NEW_ALIGN = 0xf
NEW_ARRAY_ALIGN = 0x10
DELETE_ALIGN = 0x11
DELETE_ARRAY_ALIGN = 0x12

decoder = TMtcDecoder()
print("Decoding...")
points = decoder.decode("/home/duncan/Development/Uni/Thesis/Logs/chrometrackingrepro/memory_tracker.tmtc")
addresses = {}
print(f"Decoded {points.size} points!")

print("Generating address map")
time = 0
for point in points:
    p, t = load_from_point(point, time)
    if p is None:
        continue
    time += t
    match point.key:
        case 0x0 | 0x1 | 0x2 | 0x3 | 0x5 | 0x6 | 0x7 | 0x8 | 0xf | 0x10:
            if p.ptr not in addresses:
                addresses[p.ptr] = p
        case _:
            if p.ptr in addresses:
                del addresses[p.ptr]


for p in addresses.values():
    print(p)

print(len(addresses))

print(json.dumps(addresses, default=lambda obj: obj.__json__() if hasattr(obj, "__json__") else str(obj)))