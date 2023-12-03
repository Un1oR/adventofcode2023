cmake_minimum_required(VERSION 3.20)
project(AdventOfCode)

set(CMAKE_CXX_STANDARD 20)

add_executable(aoc
    1.1.cpp
    1.1.h
    1.2.cpp
    1.2.h
    2.1.cpp
    2.1.h
    main.cpp)
target_compile_options(aoc
    PRIVATE
        -Wall
        -Wextra
        -Werror)
