cmake_minimum_required(VERSION 3.16)
project(mem_tracker C)


add_library(mem_tracker SHARED memory-tracker.c)

target_link_libraries(mem_tracker PRIVATE dl)

target_compile_options(mem_tracker PRIVATE -fPIC)

include_directories(../tests/test.h)

add_executable(mem_tracker_test tracker-test.c)