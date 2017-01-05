#include "vector.h"
#include <iostream>
#include <time.h>

template<typename T>
vector<T>::vector() : _size(0), real_size(0) {
    for (size_t i = 0; i < STATIC_SIZE; ++i) {
        sdata[i] = T();
    }
}

template<typename T>
vector<T>::vector(vector<T> const &other) {
    _size = other.size();
    real_size = other.real_size;
    if (real_size) {
        ddata.repair();
        ddata = other.ddata;
    } else {
        for (size_t i = 0; i < _size; ++i) {
            sdata[i] = other.sdata[i];
        }
    }
}

template<typename T>
vector<T> &vector<T>::operator=(vector<T> const &other) {
    if (&other == this)
        return *this;
    if (real_size)
        ddata.array_pointer<T>::~array_pointer();
    _size = other.size();
    real_size = other.real_size;
    if (real_size) {
        ddata.repair();
        ddata = other.ddata;
    } else {
        for (size_t i = 0; i < _size; ++i) {
            sdata[i] = other.sdata[i];
        }
    }
    return *this;
}

template<typename T>
vector<T>::~vector() {
    if (real_size)
        ddata.array_pointer<T>::~array_pointer();
}

template<typename T>
void vector<T>::ensure_capacity(size_t size) {
    if (real_size && size > real_size) {
        reserve(real_size * 2);
    } else if (real_size == 0 && size > STATIC_SIZE) {
        reserve(size);
    }
}

template<typename T>
void vector<T>::push_back(T const &value) {
    check_copy();
    if (real_size) {
        ensure_capacity(_size + 1);
        ddata[_size] = value;
    } else if (_size < STATIC_SIZE) {
        sdata[_size] = value;
    } else {
        T sdata_cp[STATIC_SIZE];
        for (size_t i = 0; i < STATIC_SIZE; i++)
            sdata_cp[i] = sdata[i];
        ddata.repair();
        ddata = array_pointer<T>(DEFAULT_SIZE);
        for (size_t i = 0; i < STATIC_SIZE; i++)
            ddata[i] = sdata_cp[i];
        ddata[_size] = value;
        real_size = DEFAULT_SIZE;
    }
    _size++;
}

template<typename T>
void vector<T>::pop_back() {
    check_copy();
    _size--;
    if (_size < real_size / 2 && _size >= DEFAULT_SIZE) {
        resize(_size);
    }
}

template<typename T>
void vector<T>::resize(size_t new_size) {
    check_copy();
    if (real_size == 0) {
        if (new_size > STATIC_SIZE) {
            array_pointer<T> new_ddata(new_size);
            for (size_t i = 0; i < _size; ++i) {
                new_ddata[i] = sdata[i];
            }
            ddata.repair();
            ddata = new_ddata;
            real_size = new_size;
        }
    } else {
        if (new_size >= real_size) {
            reserve(new_size);
            real_size = new_size;
        } else {
            array_pointer<T> new_ddata(new_size);
            new_ddata.mcp(ddata, new_size);
            ddata = new_ddata;
            real_size = new_size;
        }
    }
    if (real_size && _size < new_size) {
        ddata.mset(_size, new_size - _size);
    }
    _size = new_size;
}

template<typename T>
T &vector<T>::operator[](size_t index) {
//    if (index >= _size) throw 42;
    if (real_size) {
        check_copy();
        return ddata[index];
    } else {
        return sdata[index];
    }
}

template<typename T>
T const &vector<T>::operator[](size_t index) const {
//    if (index >= _size) throw 42;
    if (real_size) {
        return ddata[index];
    } else {
        return sdata[index];
    }
}

template<typename T>
void vector<T>::clear() {
    check_copy();
    if (real_size)
        ddata.array_pointer<T>::~array_pointer();
    _size = 0;
    real_size = 0;
}

template<typename T>
void vector<T>::reserve(size_t new_size) {
    check_copy();
    if (real_size >= new_size || new_size <= STATIC_SIZE)
        return;
    if (real_size) {
        array_pointer<T> ddata_cp = ddata;
        ddata = array_pointer<T>(new_size);
        ddata.mcp(ddata_cp, _size);
    } else {
        T sdata_cp[STATIC_SIZE];
        for (size_t i = 0; i < STATIC_SIZE; i++)
            sdata_cp[i] = sdata[i];
        ddata.repair();
        ddata = array_pointer<T>(new_size);
        for (size_t i = 0; i < STATIC_SIZE; i++)
            ddata[i] = sdata_cp[i];
    }
    real_size = new_size;
}

template<typename T>
size_t vector<T>::size() const {
    return _size;
}

template<typename T>
inline void vector<T>::check_copy() {
    if (real_size && ddata.counter() != 1) {
        array_pointer<T> new_ddata(ddata.size());
        new_ddata.mcp(ddata, ddata.size());
        ddata = new_ddata;
    }
}