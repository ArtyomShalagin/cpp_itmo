#ifndef BIGINTEGER_VECTOR_H
#define BIGINTEGER_VECTOR_H

#include <string>
#include "array_pointer.h"

template<typename T>
struct vector {
    vector();

    vector(vector const &);

    vector &operator=(vector const &);

    ~vector();

    void push_back(T const &);

    void pop_back();

    void resize(size_t);

    T &operator[](size_t);

    T const &operator[](size_t) const;

    void clear();

    void reserve(size_t);

    size_t size() const;

private:
    static const size_t STATIC_SIZE = 2;
    static const size_t DEFAULT_SIZE = 10;
    size_t _size, real_size;
    union {
        T sdata[STATIC_SIZE];
        array_pointer<T> ddata;
    };

    inline void ensure_capacity(size_t);

    inline void check_copy();
};

#include "vector.cpp"

#endif //BIGINTEGER_VECTOR_H