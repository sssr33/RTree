#pragma once

#include <string>

template<class T>
struct another_name {
    T val;

    another_name(const T &val)
        : val(val)
    {}

    another_name(T &&val)
        : val(std::move(val))
    {}

    operator T() {
        return this->val;
    }
};

typedef another_name<std::string> string_u8;