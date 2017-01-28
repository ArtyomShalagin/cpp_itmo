cmake_minimum_required(VERSION 3.6)
project(hw4)

#set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O0")

set(SOURCE_FILES main.cpp)
add_executable(hw4 ${SOURCE_FILES})