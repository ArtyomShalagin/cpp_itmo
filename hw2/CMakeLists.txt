cmake_minimum_required(VERSION 3.3)
project(BigInteger)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -fsanitize=undefined")

include_directories(${BigInteger_SOURCE_DIR})

#set(SOURCE_FILES main.cpp big_integer.h big_integer.cpp)
set(SOURCE_FILES
        big_integer.cpp
#        big_integer_testing
#        main.cpp
        big_integer_testing.cpp gtest/gtest-all.cc gtest/gtest_main.cc
#        vector.cpp
        vector.h
#        array_pointer.cpp
        array_pointer.h
#        my_vector.h
        )

add_executable(BigInteger ${SOURCE_FILES})
#add_executable(big_integer_testing
#               big_integer_testing.cpp
#               big_integer.h
#               big_integer.cpp
#               gtest/gtest-all.cc
#               gtest/gtest.h
#               gtest/gtest_main.cc)

#if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -pedantic")
#endif()
#
#target_link_libraries(big_integer_testing -lgmp -lgmpxx -lpthread)
