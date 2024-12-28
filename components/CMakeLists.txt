cmake_minimum_required(VERSION 3.16)
project(Memory-Monitor C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(COMMON_DIR ${CMAKE_SOURCE_DIR}/common)

add_subdirectory(memory-monitor)
add_subdirectory(memory-tracker)