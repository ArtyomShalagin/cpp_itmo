#include "big_integer.h"

#include <string>
#include <iostream>
#include <bitset>
#include <limits>

using std::cout;
using std::endl;

uint32_t const BASE_LEN = 32;
uint64_t const BASE = 1ull << BASE_LEN;

big_integer::big_integer() {
    val.push_back(0);
    positive = true;
}

big_integer::big_integer(big_integer const &other) {
    val = other.val;
    positive = other.positive;
}

big_integer::big_integer(int a) {
    bool min_val = a == std::numeric_limits<int>::min();
    val.push_back((uint32_t) std::abs(min_val ? a + 1 : a));
    positive = a >= 0;
    if (min_val)
        *this -= 1;
}

big_integer::big_integer(uint64_t a) {
    val.push_back((uint32_t) ((a << BASE_LEN) >> BASE_LEN));
    val.push_back((uint32_t) (a >> BASE_LEN));
    clear_zeros(*this);
    positive = true;
}

big_integer::big_integer(std::string const &str) {
    bool neg = false;
    size_t bound = 0;
    if (str[0] == '-') {
        neg = true;
        bound = 1;
    }
    big_integer curr_pow = 1;
    big_integer res;
    for (size_t i = str.size() - 1; i < str.size() && i >= bound; i--) {
        int x = std::stoi(str.substr(i, 1));
        big_integer tmp = x * curr_pow;
        res += tmp;
        curr_pow *= 10;
    }
    val = res.val;
    positive = true;
    positive = *this != 0 ? !neg : true;
}

big_integer::~big_integer() {
}

big_integer &big_integer::operator=(big_integer const &other) {
    val = other.val;
    positive = other.positive;
    return *this;
}

void big_integer::unsigned_add(big_integer &a, big_integer const &b) {
    uint64_t c = 0;
    if (a.val.size() <= b.val.size()) {
        a.val.resize(b.val.size() + 1);
    }
    for (int32_t i = 0; i < b.val.size() || c; i++) {
        uint64_t x = c + (i < b.val.size() ? b.val[i] : 0) + a.val[i];
        a.val[i] = (uint32_t) x;
        c = x >> BASE_LEN;
    }
    clear_zeros(a);
}

void big_integer::unsigned_subtract(big_integer &a, big_integer const &b) {
    if (a < b) {
        big_integer b_cp = b;
        unsigned_subtract(b_cp, a);
        val = b_cp.val;
        positive = !b_cp.positive;
        return;
    }
    int32_t c = 0;
    for (size_t i = 0; i < b.val.size() || c; i++) {
        uint32_t prev = a.val[i];
        a.val[i] -= c + (i < b.val.size() ? b.val[i] : 0);
        c = 0;
        if (a.val[i] > prev) {
            a.val[i] += BASE;
            c = 1;
        }
    }
    clear_zeros(*this);
}

big_integer abs(big_integer arg) {
    arg.positive = true;
    return arg;
}

big_integer &big_integer::operator+=(big_integer const &rhs) {
    if (positive && rhs.positive) {
        unsigned_add(*this, rhs);
    } else if (positive && !rhs.positive) {
        *this -= abs(rhs);
    } else if (!positive && rhs.positive) {
        positive = true;
        unsigned_subtract(*this, rhs);
        if (*this == 0) {
            positive = true;
        } else {
            positive = !positive;
        }
    } else {
        unsigned_add(*this, rhs);
        positive = false;
    }
    return *this;
}

big_integer &big_integer::operator-=(big_integer const &rhs) {
    if (positive && rhs.positive) {
        unsigned_subtract(*this, rhs);
    } else if (positive && !rhs.positive) {
        unsigned_add(*this, rhs);
    } else {
        positive = true;
        *this += rhs;
        if (*this == 0) {
            positive = true;
        } else {
            positive = !positive;
        }
    }
    return *this;
}

big_integer &big_integer::operator*=(big_integer const &rhs) {
    big_integer res;
    res.val.resize(val.size() + rhs.val.size());
    uint64_t c;
    for (size_t i = 0; i < val.size(); i++) {
        c = 0;
        for (size_t j = 0; j < rhs.val.size() || c; j++) {
            uint64_t curr = c + res.val[i + j] + 1ull * val[i] * (j < rhs.val.size() ? rhs.val[j] : 0);
            res.val[i + j] = (uint32_t) curr;
            c = curr >> BASE_LEN;
        }
    }
    clear_zeros(res);
    val = res.val;
    positive = !(positive ^ rhs.positive);
    return *this;
}

void big_integer::div_short(big_integer &a, big_integer const &b) {
    uint64_t c = 0;
    for (size_t i = a.val.size() - 1; i < a.val.size(); i--) {
        uint64_t curr = a.val[i] + (c << BASE_LEN);
        a.val[i] = (uint32_t) (curr / b.val[0]);
        c = curr % b.val[0];
    }
    clear_zeros(a);
}

void big_integer::div_long(big_integer &a, big_integer const &b) {
    big_integer res;
    res.val.clear();
    res.val.reserve(a.val.size());
    big_integer curr;
    curr.val.clear();
    curr.val.reserve(val.size());

    big_integer arg = b;
    arg.positive = true;
    a.positive = true;
    for (size_t i = a.val.size() - arg.val.size(); i < a.val.size(); i++) {
        curr.val.push_back(a.val[i]);
    }
    curr >>= BASE_LEN;
    int32_t arg_size = (int32_t) arg.val.size();
    for (int32_t i = (int32_t) a.val.size() - 1; i >= arg_size - 1; i--) {
        curr <<= BASE_LEN;
        curr.val[0] = a.val[i - arg_size + 1];
        uint64_t l = 0, r = BASE;

        bool last_l = false;
        if (curr < arg) {
            r = 1;
            last_l = true;
        }
        big_integer mid;
        uint64_t m_prev = 0;
        int cnt = BASE_LEN - 1;
        while (l != r - 1) {
            uint64_t m = (l + r) >> 1;
            arg <<= cnt;
            if (m > m_prev) {
                mid += arg;
            } else {
                mid -= arg;
            }
            arg >>= cnt;
            if (mid <= curr) {
                l = m;
                last_l = true;
            } else {
                r = m;
                last_l = false;
            }
            m_prev = m;
            cnt--;
        }

        curr -= (last_l ? mid : mid - arg);
        res.val.push_back((uint32_t) l);
    }

    a.val.clear();
    a.val.reserve(res.val.size());
    for (size_t i = res.val.size() - 1; i < res.val.size(); i--) {
        a.val.push_back(res.val[i]);
    }
    clear_zeros(*this);
}

big_integer &big_integer::operator/=(big_integer const &rhs) {
    if (abs(*this) < abs(rhs)) {
        val.clear();
        val.push_back(0);
        positive = true;
        return *this;
    }
    bool prev_positive = positive;
    if (rhs.val.size() == 1) {
        div_short(*this, rhs);
    } else {
        div_long(*this, rhs);
    }
    positive = !(prev_positive ^ rhs.positive);

    return *this;
}

big_integer &big_integer::operator%=(big_integer const &rhs) {
    big_integer a(*this);
    a = a / rhs * rhs;
    return *this -= a;
}

void big_integer::bit_operation(big_integer const &rhs, int type) {
    big_integer arg = rhs;
    bool res_positive = true;
    switch (type) {
        case 0:
            res_positive = !(!positive & !rhs.positive);
            break;
        case 1:
            res_positive = !(!positive | !rhs.positive);
            break;
        case 2:
            res_positive = !(!positive ^ !rhs.positive);
            break;
        default:
            std::cerr << "undefined bit operation type\n";
    }
    if (!rhs.positive) {
        arg = unsigned_not(arg);
        unsigned_add(arg, 1);
    }
    if (!positive) {
        *this = unsigned_not(*this);
        unsigned_add(*this, 1);
    }
    while (arg.val.size() < val.size()) {
        arg.val.push_back(0);
    }
    while (val.size() < arg.val.size()) {
        val.push_back(0);
    }
    for (size_t i = 0; i < val.size(); i++) {
        switch (type) {
            case 0:
                val[i] &= arg.val[i];
                break;
            case 1:
                val[i] |= arg.val[i];
                break;
            case 2:
                val[i] ^= arg.val[i];
                break;
            default:
                std::cerr << "undefined bit operation type\n";
        }
    }

    if (!res_positive) {
        *this = unsigned_not(*this);
        unsigned_add(*this, 1);
    }
    positive = res_positive;
    clear_zeros(*this);
}

big_integer &big_integer::operator&=(big_integer const &rhs) {
    bit_operation(rhs, 0);
    return *this;
}

big_integer &big_integer::operator|=(big_integer const &rhs) {
    bit_operation(rhs, 1);
    return *this;
}

big_integer &big_integer::operator^=(big_integer const &rhs) {
    bit_operation(rhs, 2);
    return *this;
}

big_integer &big_integer::operator<<=(int rhs) {
    int shift = rhs / BASE_LEN;
    for (size_t i = 0; i <= shift; i++) {
        val.push_back(0);
    }
    rhs %= BASE_LEN;
    for (int32_t i = (int32_t) (val.size() - 2); i >= shift; i--) {
        // >> 32 is undefined behavior
        val[i + 1] |= rhs == 0 ? 0 : (val[i - shift] >> (BASE_LEN - rhs));
        val[i] = 0 | ((uint32_t) (val[i - shift] << rhs));
    }
    for (int32_t i = shift - 1; i >= 0; i--) {
        val[i] = 0;
    }

    clear_zeros(*this);
    return *this;
}

big_integer &big_integer::operator>>=(int rhs) {
    if (rhs >= BASE_LEN * val.size()) {
        *this = 0;
        return *this;
    }
    int32_t shift = rhs / BASE_LEN;
    rhs %= BASE_LEN;

    bool lostOnes = false;
    for (size_t i = 0; i < shift; i++) {
        lostOnes |= val[i] != 0;
    }
    lostOnes |= ((uint32_t) (val[shift] << (BASE_LEN - rhs))) != 0;

    val[0] = val[shift] >> rhs;
    for (size_t i = 0; i < val.size() - shift - 1; i++) {
        //because it is C++ and >> 32 is undefined behavior
        val[i] |= (uint32_t) (((uint64_t) val[i + shift + 1]) << (BASE_LEN - rhs));
        val[i + 1] = val[i + shift + 1] >> rhs;
    }
    for (size_t i = val.size() - shift; i < val.size(); i++) {
        val[i] = 0;
    }

    if (!positive && lostOnes) {
        *this -= 1;
    }

    clear_zeros(*this);
    return *this;
}

big_integer big_integer::operator+() const {
    return big_integer(*this);
}

big_integer big_integer::operator-() const {
    big_integer copy(*this);
    if (*this != 0) {
        copy.positive = !copy.positive;
    }
    return copy;
}

big_integer big_integer::unsigned_not(big_integer const &a) const {
    big_integer res;
    res.val.clear();
    res.val.resize(a.val.size());
    for (size_t i = 0; i < a.val.size(); i++) {
        res.val[i] = ~a.val[i] & (~((uint32_t) 0));
    }
    res.positive = !res.positive;
    return res;
}

big_integer big_integer::operator~() const {
    big_integer res(*this);
    return -res - 1;
}

big_integer &big_integer::operator++() {
    *this += 1;
    return *this;
}

big_integer big_integer::operator++(int) {
    big_integer res(*this);
    *this += 1;
    return res;
}

big_integer &big_integer::operator--() {
    *this -= 1;
    return *this;
}

big_integer big_integer::operator--(int) {
    big_integer res(*this);
    *this -= 1;
    return res;
}

big_integer operator+(big_integer a, big_integer const &b) {
    return a += b;
}

big_integer operator-(big_integer a, big_integer const &b) {
    return a -= b;
}

big_integer operator*(big_integer a, big_integer const &b) {
    return a *= b;
}

big_integer operator/(big_integer a, big_integer const &b) {
    return a /= b;
}

big_integer operator%(big_integer a, big_integer const &b) {
    return a %= b;
}

big_integer operator&(big_integer a, big_integer const &b) {
    return a &= b;
}

big_integer operator|(big_integer a, big_integer const &b) {
    return a |= b;
}

big_integer operator^(big_integer a, big_integer const &b) {
    return a ^= b;
}

big_integer operator<<(big_integer a, int b) {
    return a <<= b;
}

big_integer operator>>(big_integer a, int b) {
    return a >>= b;
}

bool operator==(big_integer const &a, big_integer const &b) {
    if (a.val.size() != b.val.size() || a.positive != b.positive) {
        return false;
    }
    for (size_t i = a.val.size() - 1; i < a.val.size(); i--) {
        if (a.val[i] != b.val[i]) {
            return false;
        }
    }
    return true;
}

bool operator!=(big_integer const &a, big_integer const &b) {
    return !(a == b);
}

bool operator<(big_integer const &a, big_integer const &b) {
    if (!a.positive && b.positive) {
        return true;
    } else if (a.positive && !b.positive) {
        return false;
    } else if (!a.positive && !b.positive) {
        return abs(a) > abs(b);
    }
    if (a.val.size() != b.val.size()) {
        return a.val.size() < b.val.size();
    }
    for (int32_t i = (int32_t) (a.val.size() - 1); i >= 0; i--) {
        if (a.val[i] != b.val[i]) {
            return a.val[i] < b.val[i];
        }
    }
    return false;
}

bool operator>(big_integer const &a, big_integer const &b) {
    if (!a.positive && b.positive) {
        return false;
    } else if (a.positive && !b.positive) {
        return true;
    } else if (!a.positive && !b.positive) {
        return abs(a) < abs(b);
    }
    if (a.val.size() != b.val.size()) {
        return a.val.size() > b.val.size();
    }
    for (int32_t i = (int32_t) (a.val.size() - 1); i >= 0; i--) {
        if (a.val[i] != b.val[i]) {
            return a.val[i] > b.val[i];
        }
    }
    return false;
}

bool operator<=(big_integer const &a, big_integer const &b) {
    return !(a > b);
}

bool operator>=(big_integer const &a, big_integer const &b) {
    return !(a < b);
}

void big_integer::clear_zeros(big_integer &bi) const {
    while (bi.val[bi.val.size() - 1] == 0 && bi.val.size() > 1) {
        bi.val.pop_back();
    }
}

std::string to_bit_string(big_integer const &a) {
    std::string res = "";
    for (size_t i = a.val.size() - 1; i < a.val.size(); i--) {
        std::bitset<32> x(a.val[i]);
        res += x.to_string();
    }
    return (a.positive ? "" : "-") + res;
}

std::string to_string(big_integer const &a) {
    std::string res = "";
    big_integer curr = a;
    while (true) {
        big_integer new_curr = curr / 10;
        big_integer val = curr - new_curr * 10;
        res = std::to_string(val.val[0]) + res;
        curr = new_curr;
        if (curr == 0) {
            break;
        }
    }
    return (a.positive ? "" : "-") + res;
}

std::ostream &operator<<(std::ostream &s, big_integer const &a) {
    return s << to_string(a);
}