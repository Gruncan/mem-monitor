import sys
from tmtcdecoder import TMtcDecoder


args = sys.argv[1:]
if len(args) != 2:
    print(f"{sys.argv[0]} <address file> <logfile>.tmtc", file=sys.stderr)
    exit(1)


address_filename = args[0]
logfile_filename = args[1]

with open(address_filename, "r") as f:
    addresses = f.read().split("\n")


decoder = TMtcDecoder(logfile_filename)


for entry in decoder.decode():
    if entry.ptr in addresses:
        print(entry)
