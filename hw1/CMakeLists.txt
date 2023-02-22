cmake_minimum_required(VERSION 3.23)
project(hw1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0")

add_executable(hw1 main.cpp tasks.cpp functions.h)
