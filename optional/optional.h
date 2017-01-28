#ifndef OPTIONAL_OPTIONAL_H
#define OPTIONAL_OPTIONAL_H

#include <functional>
#include <iostream>

struct nullopt_t {
};
constexpr nullopt_t nullopt;

struct in_place_t {
};
constexpr in_place_t in_place;

template<typename T>
struct optional {

    typedef typename std::aligned_storage<sizeof(T), alignof(T)>::type data_t;

    optional() : valid(false) {}

    optional(T const &value) : valid(true) {
        new(&data) T(value);
    }

    optional(T &&value) : valid(true) {
        new(&data) T(std::move(value));
    }

    optional(const optional &other) : valid(other.valid) {
        if (valid) {
            new(&data) T(*other);
        }
    }

    optional(optional &&other) : valid(other.valid) {
        if (valid) {
            new(&data) T(std::move(*other));
        }
    }

    optional(nullopt_t) : valid(false) {}

    template<typename... Args>
    optional(in_place_t, Args &&... args) {
        new(&data) T(std::forward<Args>(args)...);
        valid = true;
    }

    optional &operator=(optional other) {
        swap(other);
        return *this;
    }

    optional &operator=(nullopt_t) {
        valid = false;
        reinterpret_cast<T *>(&data)->T::~T();
        return *this;
    }

    ~optional() {
        if (valid) {
            reinterpret_cast<T *>(&data)->T::~T();
        }
    }

    T &operator*() {
        return reinterpret_cast<T &>(data);
    }

    T *operator->() {
        return reinterpret_cast<T *>(&data);
    }

    T const &operator*() const {
        return reinterpret_cast<T const &>(data);
    }

    T const *operator->() const {
        return reinterpret_cast<T const *>(&data);
    }

    explicit operator bool() {
        return valid;
    }

    T value_or(T &&default_value) {
        if (valid) {
            return **this;
        } else {
            return std::move(default_value);
        }
    }

    void reset() {
        valid = false;
        reinterpret_cast<T *>(&data)->T::~T();
    }

    template<typename... Args>
    void emplace(Args &&... args) {
        if (valid) {
            reinterpret_cast<T *>(&data)->T::~T();
            valid = false;
        }
        new(&data) T(std::forward<Args>(args)...);
        valid = true;
    }

    void swap(optional &other) {
        if (valid) {
            if (other) {
                std::swap(**this, *other);
            } else {
                new(&other.data) T(**this);
                other.valid = true;
                valid = false;
                reinterpret_cast<T *>(&data)->T::~T();
            }
        } else {
            if (other) {
                new(&data) T(*other);
                valid = true;
                other.valid = false;
                reinterpret_cast<T *>(&other.data)->T::~T();
            }
        }
    }

    template<typename U>
    friend bool operator<(optional<U> &first, optional<U> &second);

    template<typename U>
    friend bool operator>(optional<U> &first, optional<U> &second);

    template<typename U>
    friend bool operator<=(optional<U> &first, optional<U> &second);

    template<typename U>
    friend bool operator>=(optional<U> &first, optional<U> &second);

    template<typename U>
    friend bool operator==(optional<U> &first, optional<U> &second);

    template<typename U>
    friend bool operator!=(optional<U> &first, optional<U> &second);

    bool valid;
    data_t data;
};

template<typename T>
bool operator<(optional<T> &first, optional<T> &second) {
    if (first) {
        if (second) {
            return *first < *second;
        } else {
            return false;
        }
    } else {
        if (second) {
            return true;
        }
        return false;
    }
}

template<typename T>
bool operator>(optional<T> &first, optional<T> &second) {
    return second < first;
}

template<typename T>
bool operator<=(optional<T> &first, optional<T> &second) {
    return !(first > second);
}

template<typename T>
bool operator>=(optional<T> &first, optional<T> &second) {
    return !(first < second);
}

template<typename T>
bool operator==(optional<T> &first, optional<T> &second) {
    if (bool(first) ^ bool(second)) {
        return false;
    } else if (first && second) {
        return *first == *second;
    }
    return true;
}

template<typename T>
bool operator!=(optional<T> &first, optional<T> &second) {
    return !(first == second);
}

#endif