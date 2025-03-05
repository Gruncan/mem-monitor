import os
from pprint import pprint

stat_map_keys = [
    "pid",
    "command_name",
    "process_state",
    "parent_pid",
    "process_group_id",
    "session_id",
    "controlling_terminal",
    "terminal_process_group_id",
    "process_flags",
    "minflt",
    "cminflt",
    "majflt",
    "cmajflt",
    "utime",
    "stime",
    "cutime",
    "cstime",
    "priority",
    "nice",
    "num_threads",
    "itrealvalue",
    "starttime",
    "vsize",
    "rss",
    "rsslim"
]



def parse_stat(line):
    return {k:v for k, v in zip(stat_map_keys, line.split(" "))}


def get_stat_files(path):
    return [path + v for v in os.listdir(path)]

def read_content(path):
    with open(path, "r") as f:
        data = parse_stat(f.read())

    return data

def get_all_data(path):
    data = {}
    for stat_file in get_stat_files(path):
        values = read_content(stat_file)
        data[values["pid"]] = values
    return data


base_path = "/home/duncan/Development/Uni/Thesis/Data/chromerun1/"
before_path = base_path + "before/"
after_path = base_path + "after/"

all_data = {"before": get_all_data(before_path), "after": get_all_data(after_path)}


diff_values = {}
keys = ["vsize", "rss", "majflt"]

if __name__ == "__main__":
    for k, v in all_data.items():
        for pid, values in v.items():
            if diff_values.get(pid) is None:
                diff_values[pid] = {key: values[key] for key in keys}
            else:
                for key in keys:
                    diff_values[pid][key] = int(values[key]) - int(diff_values[pid][key])

    pprint(diff_values)





