cmake_minimum_required(VERSION 3.20)
project(AdventOfCode)

set(CMAKE_CXX_STANDARD 20)

add_executable(aoc
    1.1.cpp
    1.2.cpp
    2.1.cpp
    2.2.cpp
    3.1.cpp
    3.2.cpp
    4.1.cpp
    4.2.cpp
    5.1.cpp
    5.2.cpp
    6.1.cpp
    6.2.cpp
    7.1.cpp
    7.2.cpp
    8.1.cpp
    8.2.cpp
    9.1.cpp
    9.2.cpp
    10.1.cpp
    10.2.cpp
    11.1.cpp
    11.2.cpp
    12.1.cpp
    12.2.cpp
    13.1.cpp
    13.2.cpp
    14.1.cpp
    14.2.cpp
    15.1.cpp
    15.2.cpp
    16.1.cpp
    16.2.cpp
    h.h
    main.cpp)
target_compile_options(aoc
    PRIVATE
        -Wall
        -Wextra
        -Werror)
