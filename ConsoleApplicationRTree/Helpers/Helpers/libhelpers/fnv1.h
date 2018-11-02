#pragma once

#include <cstdint>

/*
https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function

Taken here:
https://gist.github.com/filsinger/1255697/21762ea83a2d3c17561c8e6a29f44249a4626f9e

There is also another implementation but it uses strlen.
https://gist.github.com/hwei/1950649d523afd03285c
*/
namespace hash {
    template <typename S> struct fnv_internal;
    template <typename S> struct fnv1;
    template <typename S> struct fnv1a;

    template <> struct fnv_internal<uint32_t> {
        constexpr static uint32_t default_offset_basis = 0x811C9DC5;
        constexpr static uint32_t prime = 0x01000193;
    };

    template <> struct fnv1<uint32_t> : public fnv_internal<uint32_t> {
        constexpr static inline uint32_t hash(char const*const aString, const uint32_t val = default_offset_basis) {
            return (aString[0] == '\0') ? val : hash(&aString[1], static_cast<uint32_t>(val * static_cast<uint64_t>(prime)) ^ uint32_t(aString[0]));
        }
    };

    template <> struct fnv1a<uint32_t> : public fnv_internal<uint32_t> {
        constexpr static inline uint32_t hash(char const*const aString, const uint32_t val = default_offset_basis) {
            return (aString[0] == '\0') ? val : hash(&aString[1], static_cast<uint32_t>(val ^ uint32_t(aString[0])) * static_cast<uint64_t>(prime));
        }
    };
} // namespace hash