cmake_minimum_required(VERSION 3.5)
project(ops2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall --pedantic-errors -Wextra -fdiagnostics-color=always -std=c++11 -DLOGIC_TYPE=Path")

set(SOURCE_FILES Point.h main.cpp)
add_executable(ops2 ${SOURCE_FILES})