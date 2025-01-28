import pytest
import subprocess


COMMAND = ["./mtc_tests", "memory_tracker1.tmtc"]

def test_tmtc_decoding_match():
    with open("out.txt", "r") as expectedFile:
        expected_lines = expectedFile.readlines()

    expected_lines = expected_lines[1:]

    result = subprocess.run(COMMAND, capture_output=True, text=True)

    actual_lines = result.stdout.split("\n")

    assert len(actual_lines) == expected_lines

    for expect, actual in zip(expected_lines, actual_lines):
        assert expect == actual



