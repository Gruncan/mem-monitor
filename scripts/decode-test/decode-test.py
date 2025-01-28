import pytest
import subprocess


COMMAND = ["./mtc_tests", "memory_tracker1.tmtc"]

def test_tmtc_decoding_match():
    with open("out.txt", "r") as expectedFile:
        expected_lines = expectedFile.read()

    expected_lines = expected_lines.split("\n")[1:]
    result = subprocess.run(COMMAND, capture_output=True, text=True)

    actual_lines = result.stdout.split("\n")
    print(f"Actual lines: {len(actual_lines)} | Expected lines: {len(expected_lines)}")
    assert len(actual_lines) == len(expected_lines)

    for expect, actual in zip(expected_lines, actual_lines):
        print(expect)
        print(actual)
        assert expect == actual
        print()

