#include <iostream>
#include <string>
#include <random>
#include <assert.h>
#include "optional.h"

using std::cout;
using std::endl;
using std::string;

template<typename T>
optional<T> get_rvalue(T value) {
    return optional<T>(value);
}

struct throwing_type {
    int value;

    throwing_type(int value) {
        this->value = value;
    }

    throwing_type(throwing_type const& other) {
        value = other.value;
        throw "world in fire";
    }
};

void test() {
    { // test ctors
        optional<int> o1; // empty
        int foo = 42;
        optional<int> o2(foo); // const&
        optional<int> o3(123); // T rvalue
        optional<int> o4(o3); // optional&
        optional<int> o5(get_rvalue(12345)); // optional rvalue
        optional<int> o6(nullopt); // nullopt
        optional<int> o7(in_place, 321); // in place constr
        assert(!o1);
        assert(*o2 == foo);
        assert(*o3 == 123);
        assert(o4 == o3);
        assert(*o5 == 12345);
        assert(!o6);
        assert(*o7 == 321);
        cout << "test ctors done" << endl;
    }
    { // operator= and emplace test
        optional<int> o1;
        o1.emplace(123);
        assert(*o1 == 123);
        optional<int> o2(321);
        o2 = o1;
        assert(*o2 == 123);
        o2 = get_rvalue(321);
        assert(*o2 == 321);
        o2 = nullopt;
        assert(!o2);
        assert(o2.value_or(42) == 42);
        assert(o1.value_or(42) == 123);
        o1.reset();
        assert(!o1);
        optional<int> o3(get_rvalue(42));
        assert(*o3 == 42);
        cout << "test operator= and emplace done" << endl;
    }
    { // compare and swap test
        optional<int> o1(10);
        optional<int> o2(20);
        optional<int> o3(20);
        assert(o1 < o2);
        assert(o2 == o3);
        assert(o3 >= o1);
        o3 = nullopt;
        assert(o3 < o1);
        o1.swap(o2);
        assert(o1 > o2);
        cout << "compare and swap test done" << endl;
    }
    { // exception in ctor
//        throwing_type thr(123);
//        optional<throwing_type> o1(thr);
//        o1 = optional<throwing_type>(in_place, 321);
//        cout << ((*o1).value) << endl;
    }
}

int main() {
    test();

    return 0;
}