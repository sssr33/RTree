#pragma once

#include <string>
#include <codecvt>

template<class T>
const T *UtfEndFinder(const T *str) {
    while (*(++str) != (T)0);
    return str;
}

template<class Out, class In, class CodecvtFn>
size_t CodeCvtLength(const In *start, const In *end, CodecvtFn &codecvtFn) {
    size_t length = 0;
    char outTmp[16]; // 16 bytes should be enough for long time for it can hold two 8-byte codepoints.
    Out *outStart = (Out*)outTmp;
    size_t outCount = sizeof(outTmp) / sizeof(Out);
    Out *outEnd = outStart + outCount;
    Out *outNext = nullptr;
    const In *next = nullptr;
    bool ended = false;

    while (start != end && !ended) {
        auto res = codecvtFn(start, end, next, outStart, outEnd, outNext);

        switch (res) {
        case std::codecvt_base::noconv:
            length = end - start; // to_next is set to be equal to to, state is unchanged, and std::codecvt_base::noconv is returned. http://en.cppreference.com/w/cpp/locale/codecvt/in
            ended = true;
            break;
        case std::codecvt_base::ok:
        case std::codecvt_base::partial:
            length += (size_t)(outNext - outStart);
            break;
        case std::codecvt_base::error:
        default:
            length = -1;
            ended = true;
            break;
        }

        if (next == start) {
            length = -1;
            ended = true;
        }

        start = next;
    }

    return length;
}

template<class Out, class In, class Codecvt>
size_t CodeCvtLengthIn(const In *start, const In *end, Codecvt &codecvt, std::mbstate_t &mb = std::mbstate_t()) {
    typedef typename Codecvt::extern_type In2;
    typedef typename Codecvt::intern_type Out2;

    size_t length = CodeCvtLength<Out>(start, end,
        [&](const In *start2, const In *end2, const In *&next2, Out *startOut2, Out *endOut2, Out *&nextOut2) {
        return codecvt.in(mb, (const In2*)start2, (const In2*)end2, (const In2*&)next2, (Out2*)startOut2, (Out2*)endOut2, (Out2*&)nextOut2);
    });

    return length;
}

template<class Out, class In, class Codecvt>
size_t CodeCvtLengthOut(const In *start, const In *end, Codecvt &codecvt, std::mbstate_t &mb = std::mbstate_t()) {
    typedef typename Codecvt::intern_type In2;
    typedef typename Codecvt::extern_type Out2;

    size_t length = CodeCvtLength<Out>(start, end,
        [&](const In *start2, const In *end2, const In *&next2, Out *startOut2, Out *endOut2, Out *&nextOut2) {
        return codecvt.out(mb, (const In2*)start2, (const In2*)end2, (const In2*&)next2, (Out2*)startOut2, (Out2*)endOut2, (Out2*&)nextOut2);
    });

    return length;
}

template<class Res, class In, class Codecvt>
Res ConvertToIn(const In *start, const In *end, Codecvt &codecvt, std::mbstate_t &mb = std::mbstate_t()) {
    typedef typename Codecvt::extern_type In2;
    typedef typename Codecvt::intern_type Out2;

    Res res;

    size_t length = CodeCvtLengthIn<Out2>(start, end, codecvt, mb);

    if (length != -1) {
        const In2 *next = nullptr;
        Out2 *nextOut = nullptr;

        res.resize(length);
        auto cvtRes = codecvt.in(mb, (const In2*)start, (const In2*)end, (const In2*&)next, (Out2*)&res[0], (Out2*)&res[length], (Out2*&)nextOut);

        if (cvtRes != std::codecvt_base::ok) {
            res.clear();
        }
    }

    return res;
}

template<class Res, class In, class Codecvt>
Res ConvertToOut(const In *start, const In *end, Codecvt &codecvt, std::mbstate_t &mb = std::mbstate_t()) {
    typedef typename Codecvt::intern_type In2;
    typedef typename Codecvt::extern_type Out2;
    typedef typename Res::traits_type::char_type CharType;
    Res res;

    size_t length = CodeCvtLengthOut<CharType>(start, end, codecvt, mb);

    if (length != -1) {
        const In2 *next = nullptr;
        Out2 *nextOut = nullptr;

        res.resize(length);
        auto cvtRes = codecvt.out(mb, (const In2*)start, (const In2*)end, (const In2*&)next, (Out2*)&res[0], (Out2*)&res[length], (Out2*&)nextOut);

        if (cvtRes != std::codecvt_base::ok) {
            res.clear();
        }
    }

    return res;
}

inline std::u32string ConvertToUtf32(const char *start, const char *end, std::mbstate_t &mb = std::mbstate_t()) {
    std::codecvt_utf8<uint32_t> cvt;
    return ConvertToIn<std::u32string>(start, end, cvt, mb);
}

inline std::u32string ConvertToUtf32(const char *str, std::mbstate_t &mb = std::mbstate_t()) {
    return ConvertToUtf32(str, str + strlen(str), mb);
}

inline std::u32string ConvertToUtf32(const std::string &str, std::mbstate_t &mb = std::mbstate_t()) {
    return ConvertToUtf32(&str[0], &str[str.size()], mb);
}

inline std::u32string ConvertToUtf32(const wchar_t *start, const wchar_t *end, std::mbstate_t &mb = std::mbstate_t()) {
    std::codecvt_utf16<uint32_t, 0x10ffff, std::little_endian> cvt; // default is big endian. http://en.cppreference.com/w/cpp/locale/codecvt_mode
    return ConvertToIn<std::u32string>(start, end, cvt, mb);
}

inline std::u32string ConvertToUtf32(const wchar_t *str, std::mbstate_t &mb = std::mbstate_t()) {
    return ConvertToUtf32(str, UtfEndFinder(str), mb);
}

inline std::u32string ConvertToUtf32(const std::wstring &str, std::mbstate_t &mb = std::mbstate_t()) {
    return ConvertToUtf32(&str[0], &str[str.size()], mb);
}

inline std::wstring ConvertToUtf16(const char *start, const char *end, std::mbstate_t &mb = std::mbstate_t()) {
    std::codecvt_utf8_utf16<wchar_t> cvt;
    return ConvertToIn<std::wstring>(start, end, cvt, mb);
}

inline std::wstring ConvertToUtf16(const char *str, std::mbstate_t &mb = std::mbstate_t()) {
    return ConvertToUtf16(str, str + strlen(str), mb);
}

inline std::wstring ConvertToUtf16(const std::string &str, std::mbstate_t &mb = std::mbstate_t()) {
    return ConvertToUtf16(&str[0], &str[str.size()], mb);
}

inline std::wstring ConvertToUtf16(const char32_t *start, const char32_t *end, std::mbstate_t &mb = std::mbstate_t()) {
    std::codecvt_utf16<uint32_t, 0x10ffff, std::little_endian> cvt; // default is big endian. http://en.cppreference.com/w/cpp/locale/codecvt_mode
    return ConvertToOut<std::wstring>(start, end, cvt, mb);
}

inline std::wstring ConvertToUtf16(const char32_t *str, std::mbstate_t &mb = std::mbstate_t()) {
    return ConvertToUtf16(str, UtfEndFinder(str), mb);
}

inline std::wstring ConvertToUtf16(const std::u32string &str, std::mbstate_t &mb = std::mbstate_t()) {
    return ConvertToUtf16(&str[0], &str[str.size()], mb);
}

inline std::string ConvertToUtf8(const wchar_t *start, const wchar_t *end, std::mbstate_t &mb = std::mbstate_t()) {
    std::codecvt_utf8_utf16<wchar_t, 0x10ffff, std::little_endian> cvt; // default is big endian. http://en.cppreference.com/w/cpp/locale/codecvt_mode
    return ConvertToOut<std::string>(start, end, cvt, mb);
}

inline std::string ConvertToUtf8(const wchar_t *str, std::mbstate_t &mb = std::mbstate_t()) {
    return ConvertToUtf8(str, UtfEndFinder(str), mb);
}

inline std::string ConvertToUtf8(const std::wstring &str, std::mbstate_t &mb = std::mbstate_t()) {
    return ConvertToUtf8(&str[0], &str[str.size()], mb);
}

inline std::string ConvertToUtf8(const char32_t *start, const char32_t *end, std::mbstate_t &mb = std::mbstate_t()) {
    std::codecvt_utf8<uint32_t, 0x10ffff, std::little_endian> cvt; // default is big endian. http://en.cppreference.com/w/cpp/locale/codecvt_mode
    return ConvertToOut<std::string>(start, end, cvt, mb);
}

inline std::string ConvertToUtf8(const char32_t *str, std::mbstate_t &mb = std::mbstate_t()) {
    return ConvertToUtf8(str, UtfEndFinder(str), mb);
}

inline std::string ConvertToUtf8(const std::u32string &str, std::mbstate_t &mb = std::mbstate_t()) {
    return ConvertToUtf8(&str[0], &str[str.size()], mb);
}