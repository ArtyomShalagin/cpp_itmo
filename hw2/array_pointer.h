#ifndef BIGINTEGER_ARRAY_POINTER_H
#define BIGINTEGER_ARRAY_POINTER_H

#include <cstdlib>

template<typename T>
struct array_pointer {
    array_pointer();

    array_pointer(size_t);

    array_pointer(array_pointer const &);

    array_pointer &operator=(array_pointer const &);

    ~array_pointer();

    T &operator[](size_t);

    T const &operator[](size_t) const;

    size_t size() const;

    size_t counter() const;

    void repair();

    void mcp(array_pointer &, size_t);

    void mset(size_t, size_t);

private:
    static const size_t offset_bytes = 16;
    bool valid;
    size_t offset_elements;
    T *data;

    inline uint64_t *_ref_cnt() const;

    inline uint64_t *_size() const;

    inline size_t _offset_elements(size_t) const;

    void del();
};

#include "array_pointer.cpp"

#endif //BIGINTEGER_ARRAY_POINTER_H
