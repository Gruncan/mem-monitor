import json
from pprint import pprint

from matplotlib.ticker import MaxNLocator

from tmtc_meta import *

from tmtc_decoder_wrapper import TMtcDecoder
import matplotlib.pyplot as plt
import json

ALLOCATION_CLASSES = (Malloc, Calloc, ReAlloc, ReAllocArray, New, NewNoThrow, NewArray, NewArrayNoThrow, NewAligned, NewArrayAlign)

to_json = lambda obj: obj.__json__() if hasattr(obj, "__json__") else str(obj)


def load_data(filename=None):
    decoder = TMtcDecoder()
    print("Decoding...")
    points = decoder.decode("/home/duncan/Development/Uni/Thesis/Data/uwb_test_debug_onerun_cropped.tmtc")
    addresses_lifetime = {}
    print(f"Decoded {points.size} points!")

    print("Generating address map")
    time = 0
    size = 0
    max_size = 0
    sizes = []
    times = []
    for point in list(points):
        p, t = load_from_point(point, time)
        if p is None:
            continue
        time += t

        match point.key:
            case 0x0 | 0x1 | 0x2 | 0x3 | 0x5 | 0x6 | 0x7 | 0x8 | 0xf | 0x10:
                if p.ptr not in addresses_lifetime:
                    if p.size >= 2 * 1024 * 1024 * 1024:
                        print(f"Huge allocation | ptr: {p.ptr}, size: {p.size} {p.timestamp} ({p.__class__.__name__})", )
                        continue
                    addresses_lifetime[p.ptr] = p
                    if p.size > max_size:
                        # print(f"New max | ptr: {p.ptr}, size: {p.size} ({p.__class__.__name__})", )
                        max_size = p.size
                    size += p.size
                    sizes.append(size)
                    times.append(time)
            case _:
                if p.ptr in addresses_lifetime:
                    p = addresses_lifetime[p.ptr] #.timestamp_end = p.timestamp
                    size -= p.size
                    del addresses_lifetime[p.ptr]
                    sizes.append(size)
                    times.append(time)


    if filename is not None:
        with open("address.json", "w") as f:
            json.dump(addresses_lifetime, f, default=to_json)


    # json_data = json.dumps(addresses_lifetime, default=to_json)
    # return json.loads(json_data)
    return times, sizes

def plot_size_vs_lifetime(data):

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

    plt.show()

def plot_unfreed_vs_lifetime(data):

    timestamps = []
    sizes = []

    for entry in data:
        timestamps.append(entry.timestamp)
        sizes.append(entry.size)
    # for ptr, entry in data.items():
    #     timestamps.append(entry.timestamp)
    #     sizes.append(entry.size)

    timestamps_sec = [t / 1_000_000 for t in timestamps]

    plt.figure(figsize=(12, 8))

    plt.scatter(timestamps_sec, sizes, color='blue', alpha=0.7, s=20)
    plt.yscale('log')

    plt.title('Memory Allocation Size vs Time')
    plt.xlabel('Time (seconds)')
    plt.ylabel('Memory Size (bytes)')
    plt.grid(True, which="both", ls="-")

    # Add some basic statistics as text
    plt.text(0.02, 0.95, f"Total Allocations: {len(sizes)}",
             transform=plt.gca().transAxes, fontsize=10,
             verticalalignment='top', bbox=dict(boxstyle='round', facecolor='white', alpha=0.8))

    plt.tight_layout()
    plt.show()

def plot_process_allocations_lifetime(times, sizes):
    timestamps_sec = [t / 1_000_000 for t in times]
    sizes_mb = [s / 1024 ** 3 for s in sizes]

    plt.figure(figsize=(16, 8))

    plt.plot(timestamps_sec, sizes_mb, color='blue', alpha=0.7)
    plt.title("BBMPP library allocation size vs Time")
    plt.ylabel('Size (Gb)')
    plt.xlabel('Time (seconds)')
    plt.gca().xaxis.set_major_locator(MaxNLocator(15))
    plt.show()
    plt.savefig('bbmpp_library_allocation_start.pdf')


def load_from_raw_log(filename):
    bytes_to_read = 671088640 // 20
    with open(filename, "r") as f:
        # f.seek(0, 2)
        # file_size = f.tell()
        # print(file_size)
        #
        # start_pos = max(file_size - bytes_to_read, 0)
        # f.seek(25000)
        points = parse_memory_logs(f.read())

    times = []
    sizes = []
    size = 0
    return points

    # return address_map

# data = load_from_raw_log("/home/duncan/Development/C/mem-monitor/components/common/mtc/uwb_test_raw.txt")
# data = load_from_raw_log("/home/duncan/Development/C/mem-monitor/components/common/mtc/raw_lifetime.txt")
data = load_from_raw_log("/home/duncan/Development/C/mem-monitor/components/common/mtc/raw_lifetime_single.txt")
# with open("full_collapsed.json", "w") as f:
#     json.dump(data, f, default=to_json)
# data = load_from_raw_log("/home/duncan/Development/C/mem-monitor/components/common/mtc/singlerun.txt")

# with open("/home/duncan/Development/C/mem-monitor/components/common/mtc/singlerun.txt", "r") as f:
#     max_size = get_peak_memory_raw_log(f.read())

# print(max_size / 1024 ** 3)

# plot_process_allocations_lifetime(*data)



# with open("addresses_from_raw.json", "r") as f:
#     data = json.load(f)
#
plot_unfreed_vs_lifetime(data)
