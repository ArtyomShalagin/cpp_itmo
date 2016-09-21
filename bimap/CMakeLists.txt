cmake_minimum_required(VERSION 3.3)
project(bimap)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES tester.cpp bimap.cpp bimap.h)
add_executable(bimap ${SOURCE_FILES})