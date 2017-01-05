#include "array_pointer.h"
#include <memory.h>
#include <iostream>

template<typename T>
array_pointer<T>::array_pointer() {
    valid = false;
}

template<typename T>
array_pointer<T>::array_pointer(size_t size) {
    // |size|ref_cnt|data|
    offset_elements = _offset_elements(sizeof(T));
    data = new T[size + offset_elements];
    *_size() = size;
    *_ref_cnt() = 1;
    valid = true;
}

template<typename T>
array_pointer<T>::array_pointer(array_pointer<T> const &other) {
    offset_elements = other.offset_elements;
    data = const_cast<T *>(other.data);
    (*_ref_cnt())++;
    valid = true;
}

template<typename T>
array_pointer<T> &array_pointer<T>::operator=(array_pointer<T> const &other) {
    if (&other == this)
        return *this;
    if (valid)
        del();
    offset_elements = other.offset_elements;
    data = const_cast<T *>(other.data);
    (*_ref_cnt())++;
    valid = true;
    return *this;
}

template<typename T>
array_pointer<T>::~array_pointer() {
    if (valid)
        del();
}

template<typename T>
T &array_pointer<T>::operator[](size_t index) {
    return data[index + offset_elements];
}

template<typename T>
T const &array_pointer<T>::operator[](size_t index) const {
    return data[index + offset_elements];
}

template<typename T>
size_t array_pointer<T>::size() const {
    return *_size();
}

template<typename T>
inline uint64_t *array_pointer<T>::_ref_cnt() const {
    return ((uint64_t *) data) + 1;
}

template<typename T>
inline uint64_t *array_pointer<T>::_size() const {
    return ((uint64_t *) data);
}

template<typename T>
inline size_t array_pointer<T>::_offset_elements(size_t element_size) const {
    return offset_bytes % element_size == 0 ?
           offset_bytes / element_size : offset_bytes / element_size + 1;
}

template<typename T>
size_t array_pointer<T>::counter() const {
    return *_ref_cnt();
}

template<typename T>
void array_pointer<T>::repair() {
    valid = false;
}

template<typename T>
void array_pointer<T>::mcp(array_pointer<T> &other, size_t len) {
    if (size() < len || other.size() < len) throw 42;
    memcpy(data + offset_elements, other.data + other.offset_elements, len * sizeof(T));
}

template<typename T>
void array_pointer<T>::mset(size_t from, size_t len) {
    memset((void *) (data + offset_elements + from), T(), len * sizeof(T));
}

// array pointer must be valid when this method is called
template<typename T>
void array_pointer<T>::del() {
    (*_ref_cnt())--;
    if (*_ref_cnt() == 0) {
        delete[] data;
        valid = false;
    }
}