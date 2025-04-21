import pytest
import subprocess


COMMAND = ["./tmtc_tests", "memory_tracker1.tmtc"]

COMMAND_STREAM = ["./tmtc_stream_tests", "memory_tracker1.tmtc"]

def test_tmtc_decoding_match():
    with open("out.txt", "r") as expectedFile:
        expected_lines = expectedFile.read()

    # Ignoring the first line since this is an internal malloc from some stdlib
    expected_lines = expected_lines.split("\n")[1:]
    result = subprocess.run(COMMAND, capture_output=True, text=True)

    actual_lines = result.stdout.split("\n")
    assert len(expected_lines) == len(actual_lines)

    for expect, actual in zip(expected_lines, actual_lines):
        assert expect == actual


def test_tmtc_stream_decoding_match():
    tmtc_results = subprocess.run(COMMAND, capture_output=True, text=True)
    tmtc_stream_results = subprocess.run(COMMAND_STREAM, capture_output=True, text=True)

    expected_lines = tmtc_results.stdout.split("\n")
    actual_lines = tmtc_stream_results.stdout.split("\n")

    assert len(expected_lines) == len(actual_lines)

    for expect, actual in zip(expected_lines, actual_lines):
        assert expect == actual
