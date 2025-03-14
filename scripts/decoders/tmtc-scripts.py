import json
from pprint import pprint

from tmtc_meta import *

from tmtc_decoder_wrapper import TMtcDecoder
import matplotlib.pyplot as plt
import json


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


def load_data():
    decoder = TMtcDecoder()
    print("Decoding...")
    points = decoder.decode("/home/duncan/Development/Uni/Thesis/Data/uwb_test_debug_onerun_cropped.tmtc")
    addresses_lifetime = {}
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
                if p.ptr not in addresses_lifetime:
                    addresses_lifetime[p.ptr] = p
            case _:
                if p.ptr in addresses_lifetime:
                    del addresses_lifetime[p.ptr] #.timestamp_end = p.timestamp



    with open("address.json", "w") as f:
        json.dump(addresses_lifetime, f, default=lambda obj: obj.__json__() if hasattr(obj, "__json__") else str(obj))


def plot_size_vs_lifetime(filename):
    with open(filename, 'r') as file:
        data = json.load(file)

    sizes = []
    lifetimes = []

    for key, value in data.items():
        size = value.get('size')
        if value["timestamp_end"] is None:
            continue
        lifetime = value.get('timestamp_end', 0) - value.get('timestamp', 0)

        sizes.append(size)
        lifetimes.append(lifetime)

    plt.figure(figsize=(10, 6))
    plt.scatter(lifetimes, sizes, alpha=0.7)

    plt.xlabel('Lifetime (timestamp_end - timestamp_start)')
    plt.ylabel('Size (bytes)')
    plt.title('Memory Allocation: Size vs. Lifetime')

    plt.grid(True, alpha=0.3)

    plt.savefig('size_vs_lifetime.png')

    # Show the plot
    plt.show()

plot_size_vs_lifetime("address_lifetime.json")

#
# import matplotlib.pyplot as plt
# import json
# import numpy as np
#
# # Load data
# # data = json.load(open("address.json"))
#
# data = addresses_lifetime
#
#
#
# timestamps = []
# sizes = []
#
# # Process your data
# for ptr, entry in data.items():
#     timestamps.append(entry.timestamp)
#     sizes.append(entry.size)
#
# # Convert timestamps to seconds (from microseconds) for better readability
# timestamps_sec = [t / 1_000_000 for t in timestamps]
#
# # Create the log scale plot
# plt.figure(figsize=(12, 8))
#
# # Plot with log scale on y-axis
# plt.scatter(timestamps_sec, sizes, color='blue', alpha=0.7, s=20)
# plt.yscale('log')  # Set y-axis to logarithmic scale
#
# # Add labels and grid
# plt.title('Memory Allocation Size vs Time (Log Scale)')
# plt.xlabel('Time (seconds from start)')
# plt.ylabel('Memory Size (bytes) - Log Scale')
# plt.grid(True, which="both", ls="-")
#
# # Add some basic statistics as text
# plt.text(0.02, 0.95, f"Total Allocations: {len(sizes)}",
#          transform=plt.gca().transAxes, fontsize=10,
#          verticalalignment='top', bbox=dict(boxstyle='round', facecolor='white', alpha=0.8))
#
# plt.tight_layout()
# plt.savefig("uwbsinglerun_cropped.png")