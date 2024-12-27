import re
import pytest


FILENAME = "memory_tracker.tmtc"
ADDRESS_PATTERN = r"0x[0-9a-fA-F]+"

def test_memory_tracker_data():
    with open(FILENAME, "r") as f:
        lines = f.readlines()

    lines = lines[2:-1]
    content = "".join(lines)
    # Strip first, second and last lines
    memory_patterns = {
        "malloc": r"malloc\((\d+)\) = (0x[0-9a-fA-F]+)",
        "realloc": r"realloc\((0x[0-9a-fA-F]+), (\d+)\) = (0x[0-9a-fA-F]+)",
        "free": r"free\((0x[0-9a-fA-F]+)\)",
        "calloc": r"calloc\((\d+), (\d+)\) = (0x[0-9a-fA-F]+)",
    }

    order = ["malloc", "realloc", "free", "calloc"]
    i = 0
    for line in lines:
        assert order[i] in line
        i += 1

    matches = {key: re.findall(value, content) for key, value in memory_patterns.items()}

    fm = {}
    for key, values in matches.items():
        if isinstance(values[0], str):
            values[0] = (values[0],)
        l = []
        for lines in values:
            l.append([value for value in lines if re.match(ADDRESS_PATTERN, value) is not None])
        fm[key] = l


    x = len(fm["malloc"])
    for i in range(x):
        address = fm["malloc"][i][0]
        assert address == fm["realloc"][i][0] == fm["realloc"][i][1] == fm["free"][i][0]





# test_memory_tracker_data()



