cmake_minimum_required(VERSION 3.6)
project(optional)

#set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -fsanitize=undefined -O0")

set(SOURCE_FILES main.cpp optional.h)
add_executable(optional ${SOURCE_FILES})