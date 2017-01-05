#include <iostream>
#include "vector.h"
#include <vector>
#include <random>
#include <chrono>

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::time_point<Clock> Time_point;

#define int int32_t
#define uint uint32_t
#define long int64_t
#define ulong uint64_t

using std::cout;
using std::endl;

Time_point start = Clock::now();

void retime() {
    start = Clock::now();
}

void measure(std::string name) {
    Time_point curr_time = Clock::now();
    long time = std::chrono::duration_cast<std::chrono::milliseconds>(curr_time - start).count();
    cout << name << " : " << time << "ms" << endl;
    retime();
}

ulong *generate(size_t size) {
    ulong *data = new ulong[size];
    for (size_t i = 0; i < size; i++) {
        data[i] = (ulong) (rand() * rand());
    }
    return data;
}

void test() {
    std::vector<ulong> std_vec;
    vector<ulong> my_vec;
    size_t size = 1000000 * 10;
    ulong *data = generate(size);

    {
        retime();
        for (uint i = 0; i < size; i++) {
            std_vec.push_back(data[i]);
        }
        measure("std::vec push back");
        for (uint i = 0; i < size; i++) {
            my_vec.push_back(data[i]);
        }
        measure("my vec push back");
    }
    delete[] data;

    size_t copy_tests = 30;
    std::vector<ulong> *arr1 = new std::vector<ulong>[copy_tests];
    vector<ulong> *arr2 = new vector<ulong>[copy_tests];

    {
        retime();
        for (size_t i = 0; i < copy_tests; i++) {
            arr1[i] = std_vec;
        }
        measure("std::vec copy");
        for (size_t i = 0; i < copy_tests; i++) {
            arr2[i] = my_vec;
        }
        measure("my vec copy");
    }

    {
        for (size_t i = 0; i < copy_tests; i++) {
            arr1[i][size - 1] = i;
        }
        measure("std::vec first request");
        for (size_t i = 0; i < copy_tests; i++) {
            arr2[i][size - 1] = i;
        }
        measure("my vec first request");
    }

    {
        for (size_t i = 0; i < copy_tests; i++) {
            arr1[i][size - 1] = i;
        }
        measure("std::vec second request");
        for (size_t i = 0; i < copy_tests; i++) {
            arr2[i][size - 1] = i;
        }
        measure("my vec second request");
    }
}

int main() {
    srand(1984);
    test();

    return 0;
}