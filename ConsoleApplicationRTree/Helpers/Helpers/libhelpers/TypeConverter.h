#pragma once
#include "config.h"
#include "TypeConverterCP.h"
#include "Structs.h"
#include "another_name.h"

#include <d2d1.h>
#include <string>
#include <cstdint>

// std::wstring <- LPCSTR
template<>
struct TypeConverter<std::wstring, LPCSTR> {
    static std::wstring Convert(const LPCSTR v);
};

// D2D1_SIZE_U -> D2D1_SIZE_F
template<> struct TypeConverter<D2D1_SIZE_F, D2D1_SIZE_U> {
    static D2D1_SIZE_F Convert(const D2D1_SIZE_U &v);
};

// D2D1_COLOR_F <-> D2D1::ColorF
template<> struct TypeConverter<D2D1_COLOR_F, D2D1::ColorF> {
    static D2D1_COLOR_F Convert(const D2D1::ColorF &v);
};

template<> struct TypeConverter<D2D1::ColorF, D2D1_COLOR_F> {
    static D2D1::ColorF Convert(const D2D1_COLOR_F &v);
};

// Structs::Rgba <-> D2D1::ColorF
template<> struct TypeConverter<Structs::Rgba, D2D1::ColorF> {
    static Structs::Rgba Convert(const D2D1::ColorF &v);
};

template<> struct TypeConverter<D2D1::ColorF, Structs::Rgba> {
    static D2D1::ColorF Convert(const Structs::Rgba &v);
};

// Structs::Rgba <-> D2D1_COLOR_F
template<> struct TypeConverter<Structs::Rgba, D2D1_COLOR_F> {
    static Structs::Rgba Convert(const D2D1_COLOR_F &v);
};

template<> struct TypeConverter<D2D1_COLOR_F, Structs::Rgba> {
    static D2D1_COLOR_F Convert(const Structs::Rgba &v);
};

// Structs::Rect <-> D2D1_RECT_F
template<> struct TypeConverter<Structs::Rect, D2D1_RECT_F> {
    static Structs::Rect Convert(const D2D1_RECT_F &v);
};

template<> struct TypeConverter<D2D1_RECT_F, Structs::Rect> {
    static D2D1_RECT_F Convert(const Structs::Rect &v);
};

// Structs::Float2 <-> D2D1_POINT_2F
template<> struct TypeConverter<Structs::Float2, D2D1_POINT_2F> {
    static Structs::Float2 Convert(const D2D1_POINT_2F &v);
};

template<> struct TypeConverter<D2D1_POINT_2F, Structs::Float2> {
    static D2D1_POINT_2F Convert(const Structs::Float2 &v);
};

#if HAVE_WINRT == 1
// Windows::UI::Color <-> D2D1::ColorF
template<> struct TypeConverter<Windows::UI::Color, D2D1::ColorF> {
    static Windows::UI::Color Convert(const D2D1::ColorF &v);
};

template<> struct TypeConverter<D2D1::ColorF, Windows::UI::Color> {
    static D2D1::ColorF Convert(const Windows::UI::Color &v);
};

// Windows::UI::Color <-> D2D1_COLOR_F
template<> struct TypeConverter<Windows::UI::Color, D2D1_COLOR_F> {
    static Windows::UI::Color Convert(const D2D1_COLOR_F &v);
};

template<> struct TypeConverter<D2D1_COLOR_F, Windows::UI::Color> {
    static D2D1_COLOR_F Convert(const Windows::UI::Color &v);
};

// Platform::String ^ <-> std::wstring
template<> struct TypeConverter<Platform::String ^, std::wstring> {
    static Platform::String ^Convert(const std::wstring &v);
};

template<> struct TypeConverter<std::wstring, Platform::String ^> {
    static std::wstring Convert(Platform::String ^v);
};

// Platform::String ^ <-> std::string
template<> struct TypeConverter<Platform::String ^, std::string> {
    static Platform::String ^Convert(const std::string &v);
};

template<> struct TypeConverter<std::string, Platform::String ^> {
    static std::string Convert(Platform::String ^v);
};

// Platform::String ^ <-> string_u8
template<> struct TypeConverter<Platform::String ^, string_u8> {
    static Platform::String ^Convert(const string_u8 &v);
};

template<> struct TypeConverter<string_u8, Platform::String ^> {
    static string_u8 Convert(Platform::String ^v);
};

// Windows::Foundation::TimeSpan <-> int64_t
template<> struct TypeConverter<Windows::Foundation::TimeSpan, int64_t> {
    static  Windows::Foundation::TimeSpan Convert(const int64_t &v);
};

template<> struct TypeConverter<int64_t, Windows::Foundation::TimeSpan> {
    static int64_t Convert(Windows::Foundation::TimeSpan v);
};

// Windows::Foundation::Point <-> D2D1_Size_U
template<> struct TypeConverter<Windows::Foundation::Point, D2D1_SIZE_U> {
    static Windows::Foundation::Point Convert(D2D1_SIZE_U v);
};

template<> struct TypeConverter<D2D1_SIZE_U, Windows::Foundation::Point> {
    static D2D1_SIZE_U Convert(Windows::Foundation::Point v);
};
#endif