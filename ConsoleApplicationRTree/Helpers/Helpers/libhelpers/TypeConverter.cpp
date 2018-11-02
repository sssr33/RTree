#include "TypeConverter.h"
#include "HMath.h"
#include "HText.h"

std::wstring TypeConverter<std::wstring, std::string>::Convert(const std::string &v) {
    std::wstring tmp(v.begin(), v.end());
    return tmp;
}

std::string TypeConverter<std::string, std::wstring>::Convert(const std::wstring &v) {
    std::string tmp(v.begin(), v.end());
    return tmp;
}

std::wstring TypeConverter<std::wstring, string_u8>::Convert(const string_u8 &v) {
    return H::Text::ConvertUTF8ToWString(v.val);
}

string_u8 TypeConverter<string_u8, std::wstring>::Convert(const std::wstring &v) {
    return H::Text::ConvertWStringToUTF8(v);
}

std::wstring TypeConverter<std::wstring, LPCSTR>::Convert(const LPCSTR v) {
    size_t len = (size_t)strlen(v);
    std::wstring tmp(v, v + len);
    return tmp;
}

D2D1_SIZE_F TypeConverter<D2D1_SIZE_F, D2D1_SIZE_U>::Convert(const D2D1_SIZE_U &v) {
    D2D1_SIZE_F tmp;

    tmp.width = (float)v.width;
    tmp.height = (float)v.height;

    return tmp;
}

D2D1_COLOR_F TypeConverter<D2D1_COLOR_F, D2D1::ColorF>::Convert(const D2D1::ColorF &v) {
    D2D1_COLOR_F color;

    color.a = v.a;
    color.r = v.r;
    color.g = v.g;
    color.b = v.b;

    return color;
}

D2D1::ColorF TypeConverter<D2D1::ColorF, D2D1_COLOR_F>::Convert(const D2D1_COLOR_F &v) {
    D2D1::ColorF color(v.r, v.g, v.b, v.a);
    return color;
}

// Structs::Rgba <-> D2D1::ColorF
Structs::Rgba TypeConverter<Structs::Rgba, D2D1::ColorF>::Convert(const D2D1::ColorF &v) {
    const float scale = 255.0f;
    Structs::Rgba res;

    res.r = (uint8_t)H::Math::Clamp(v.r * scale, 0.0f, scale);
    res.g = (uint8_t)H::Math::Clamp(v.g * scale, 0.0f, scale);
    res.b = (uint8_t)H::Math::Clamp(v.b * scale, 0.0f, scale);
    res.a = (uint8_t)H::Math::Clamp(v.a * scale, 0.0f, scale);

    return res;
}

D2D1::ColorF TypeConverter<D2D1::ColorF, Structs::Rgba>::Convert(const Structs::Rgba &v) {
    const float scale = 1.0f / 255.0f;
    D2D1::ColorF res(
        (float)v.r * scale,
        (float)v.g * scale,
        (float)v.b * scale,
        (float)v.a * scale);

    return res;
}

// Structs::Rgba <-> D2D1_COLOR_F
Structs::Rgba TypeConverter<Structs::Rgba, D2D1_COLOR_F>::Convert(const D2D1_COLOR_F &v) {
    auto res = TypeConverter<Structs::Rgba, D2D1::ColorF>::Convert(D2D1::ColorF(v.r, v.g, v.b, v.a));
    return res;
}

D2D1_COLOR_F TypeConverter<D2D1_COLOR_F, Structs::Rgba>::Convert(const Structs::Rgba &v) {
    return TypeConverter<D2D1::ColorF, Structs::Rgba>::Convert(v);
}

// Structs::Rect <-> D2D1_RECT_F
Structs::Rect TypeConverter<Structs::Rect, D2D1_RECT_F>::Convert(const D2D1_RECT_F &v) {
    Structs::Rect res(v.left, v.top, v.right, v.bottom);
    return res;
}

D2D1_RECT_F TypeConverter<D2D1_RECT_F, Structs::Rect>::Convert(const Structs::Rect &v) {
    D2D1_RECT_F res = D2D1::RectF(v.left, v.top, v.right, v.bottom);
    return res;
}

// Structs::Float2 <-> D2D1_POINT_2F
Structs::Float2 TypeConverter<Structs::Float2, D2D1_POINT_2F>::Convert(const D2D1_POINT_2F &v) {
    Structs::Float2 res(v.x, v.y);
    return res;
}

D2D1_POINT_2F TypeConverter<D2D1_POINT_2F, Structs::Float2>::Convert(const Structs::Float2 &v) {
    D2D1_POINT_2F res = D2D1::Point2F(v.x, v.y);
    return res;
}

#if HAVE_WINRT == 1
Windows::UI::Color TypeConverter<Windows::UI::Color, D2D1::ColorF>::Convert(const D2D1::ColorF &v) {
    Windows::UI::Color color;

    color.A = (uint8_t)(v.a * 255.0f);
    color.R = (uint8_t)(v.r * 255.0f);
    color.G = (uint8_t)(v.g * 255.0f);
    color.B = (uint8_t)(v.b * 255.0f);

    return color;
}

D2D1::ColorF TypeConverter<D2D1::ColorF, Windows::UI::Color>::Convert(const Windows::UI::Color &v) {
    const float Scale = 1.0f / 255.0f;
    float a, r, g, b;

    a = (float)v.A * Scale;
    r = (float)v.R * Scale;
    g = (float)v.G * Scale;
    b = (float)v.B * Scale;

    auto compColor = Windows::UI::Colors::Transparent;
    if (v.A == compColor.A && v.R == compColor.R && v.G == compColor.G && v.B == compColor.B) {
        a = r = g = b = 0.0f;
    }

    return D2D1::ColorF(r, g, b, a);
};

Windows::UI::Color TypeConverter<Windows::UI::Color, D2D1_COLOR_F>::Convert(const D2D1_COLOR_F &v) {
    D2D1::ColorF tmp(v.r, v.g, v.b, v.a);
    return TypeConverter<Windows::UI::Color, D2D1::ColorF>::Convert(tmp);
}

D2D1_COLOR_F TypeConverter<D2D1_COLOR_F, Windows::UI::Color>::Convert(const Windows::UI::Color &v) {
    D2D1_COLOR_F res = TypeConverter<D2D1::ColorF, Windows::UI::Color>::Convert(v);
    return res;
}

Platform::String ^TypeConverter<Platform::String ^, std::wstring>::Convert(const std::wstring &v) {
    Platform::String ^tmp = ref new Platform::String(v.data(), (int)v.size());
    return tmp;
}

std::wstring TypeConverter<std::wstring, Platform::String ^>::Convert(Platform::String ^v) {
    std::wstring tmp(v->Data(), v->Length());
    return tmp;
}

Platform::String ^TypeConverter<Platform::String ^, std::string>::Convert(const std::string &v) {
    std::wstring tmpWStr(v.begin(), v.end());
    Platform::String ^tmp = ref new Platform::String(tmpWStr.data(), (int)tmpWStr.size());
    return tmp;
}

std::string TypeConverter<std::string, Platform::String ^>::Convert(Platform::String ^v) {
    std::wstring tmpWStr(v->Data(), v->Length());
    std::string tmp(tmpWStr.begin(), tmpWStr.end());
    return tmp;
}

Platform::String ^TypeConverter<Platform::String ^, string_u8>::Convert(const string_u8 &v) {
    auto wstr = TypeConverter<std::wstring, string_u8>::Convert(v);
    return TypeConverter<Platform::String ^, std::wstring>::Convert(wstr);
}

string_u8 TypeConverter<string_u8, Platform::String ^>::Convert(Platform::String ^v) {
    auto wstr = TypeConverter<std::wstring, Platform::String ^>::Convert(v);
    return TypeConverter<string_u8, std::wstring>::Convert(wstr);
}

Windows::Foundation::TimeSpan TypeConverter<Windows::Foundation::TimeSpan, int64_t>::Convert(const int64_t &v) {
    Windows::Foundation::TimeSpan res;
    res.Duration = v;
    return res;
}

int64_t TypeConverter<int64_t, Windows::Foundation::TimeSpan>::Convert(Windows::Foundation::TimeSpan v) {
    return v.Duration;
}

Windows::Foundation::Point TypeConverter<Windows::Foundation::Point, D2D1_SIZE_U>::Convert(D2D1_SIZE_U v) {
    Windows::Foundation::Point res;
    res.X = (float)v.width;
    res.Y = (float)v.height;
    return res;
}

D2D1_SIZE_U TypeConverter<D2D1_SIZE_U, Windows::Foundation::Point>::Convert(Windows::Foundation::Point v) {
    D2D1_SIZE_U res = { (uint32_t)std::roundf(v.X), (uint32_t)std::roundf(v.Y) };
    return res;
}
#endif