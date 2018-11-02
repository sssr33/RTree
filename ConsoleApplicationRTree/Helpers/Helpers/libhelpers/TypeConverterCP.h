#pragma once
#include "config.h"
#include "another_name.h"

#include <string>
#include <cstdint>

template<class D, class S>
struct TypeConverter {
    static D Convert(const S &v) {
        static_assert(false, "Not implemented");
        return D();
    }
};

// std::wstring <-> std::string
template<> struct TypeConverter<std::wstring, std::string> {
    static std::wstring Convert(const std::string &v);
};

template<> struct TypeConverter<std::string, std::wstring> {
    static std::string Convert(const std::wstring &v);
};

// std::wstring <-> string_u8
template<> struct TypeConverter<std::wstring, string_u8> {
    static std::wstring Convert(const string_u8 &v);
};

template<> struct TypeConverter<string_u8, std::wstring> {
    static string_u8 Convert(const std::wstring &v);
};