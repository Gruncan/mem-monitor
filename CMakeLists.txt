cmake_minimum_required(VERSION 3.16)
project(mem_monitor C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_COMPILER "/usr/bin/gcc")

include_directories("source/")

add_executable(mem_monitor main.c
        source/mem-info.c
        source/mem-writer.c
        source/mem-threading.c
        source/mem-reader.c
        source/process-reader.c
)

target_link_libraries(mem_monitor PRIVATE pthread)


add_executable(mem_monitor_test tests/test.c
        source/mem-info.c
        source/mem-writer.c
        source/mem-threading.c
        source/mem-reader.c
        source/process-reader.c
)

target_link_libraries(mem_monitor_test PRIVATE pthread)