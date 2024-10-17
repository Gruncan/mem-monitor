cmake_minimum_required(VERSION 3.29)
project(mem_monitor C)

set(CMAKE_C_STANDARD 11)


include_directories("source/")

add_executable(mem_monitor main.c
        source/mem-info.c
        )
