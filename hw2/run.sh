#!/bin/bash
echo "\n\ncompiling..."
g++5 -std=c++11 -O3 -g big_integer.cpp big_integer_testing.cpp gtest/gtest-all.cc gtest/gtest_main.cc
# echo "running..."
# ./a.out
echo "running valgrind..."
valgrind --track-origins=yes --leak-check=full ./a.out
# valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all ./a.out
