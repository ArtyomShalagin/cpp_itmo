#include <iostream>
#include <vector>
#include <functional>
#include "bind.h"

using std::cout;
using std::endl;

int sum(int a, int b, int c) {
    return a + b + c;
}

int mul(int a, int b, int c) {
    return a * b * c;
}

int assert_cnt = 0;

void assert(bool result) {
    if (!result)
        cout << "error in test " << assert_cnt << endl;
    assert_cnt++;
}

int main() {
    int a = 10, b = 20, c = 30;
    assert(bind(sum, 1, 2, 3)() == std::bind(sum, 1, 2, 3)());
    assert(bind(sum, 1, 2, ph::_1)(a) == std::bind(sum, 1, 2, std::placeholders::_1)(a));
    assert(bind(sum, ph::_1, ph::_2, ph::_3)(a, b, c)
           == std::bind(sum, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)(a, b, c));
    assert(bind(sum, ph::_1, bind(mul, ph::_2, 5, ph::_3), ph::_3)(a, b, 30)
           == std::bind(sum, std::placeholders::_1,
                        std::bind(mul, std::placeholders::_2, 5, std::placeholders::_3),
                        std::placeholders::_3)(a, b, c));

    return 0;
}
