cmake_minimum_required(VERSION 3.29)
project(mem_monitor C)

set(CMAKE_C_STANDARD 11)

add_executable(mem_monitor main.c
        mem-info.h
        mem-info.c)
