#pragma once
#include "config.h"
#include "ToString.h"

#include <vector>
#include <string>

#ifdef HAVE_VISUAL_STUDIO
#include <Windows.h>
#endif

/*
format and vformat are taken from here https://stackoverflow.com/questions/69738/c-how-to-get-fprintf-results-as-a-stdstring-w-o-sprintf#69911
*/
namespace H {
    class Text {
    public:
        static std::string UriDecode(const void *src, size_t length);

        static std::string UriEncode(const void *src, size_t length);

        static std::wstring ConvertUTF8ToWString(const std::string &utf8);

        static std::string ConvertWStringToUTF8(const std::wstring &s);

        static void BreakNetUrl(const std::wstring &url, std::wstring *protocol, std::wstring *name, std::wstring *port, std::wstring *path);

        static std::wstring GetUriVar(const std::wstring &s, const std::wstring &varName);

        static std::string format(const char *fmt, ...);
        static std::wstring format(const wchar_t *fmt, ...);

        template<class T> static std::string ToString(const T &v) {
            return ToStr::Do(v);
        }

        template < class ContainerT >
        static void tokenize(const std::string& str, ContainerT& tokens,
            const std::string& delimiters = " ", bool trimEmpty = false)
        {
            std::string::size_type pos, lastPos = 0, length = str.length();

            using value_type = typename ContainerT::value_type;
            using size_type = typename ContainerT::size_type;

            while (lastPos < length + 1)
            {
                pos = str.find_first_of(delimiters, lastPos);
                if (pos == std::string::npos)
                {
                    pos = length;
                }

                if (pos != lastPos || !trimEmpty)
                    tokens.push_back(value_type(str.data() + lastPos,
                    (size_type)pos - lastPos));

                lastPos = pos + 1;
            }
        }

        static std::vector<std::string> split(const std::string &s, const std::string &delim) {
            std::vector<std::string> elems;
            tokenize(s, elems, delim, true);
            return elems;
        }

    private:
        static std::string vformat(const char *fmt, va_list ap);
        static std::wstring vformat(const wchar_t *fmt, va_list ap);
    };
}