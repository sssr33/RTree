#pragma once

#include <type_traits>

// https://stackoverflow.com/questions/30736242/how-can-i-get-the-index-of-a-type-in-a-variadic-class-template
// https://en.cppreference.com/w/cpp/types/integral_constant

// https://stackoverflow.com/questions/20001349/return-list-of-values-between-parenthesis-10-20-30-40
// https://stackoverflow.com/questions/1613230/uses-of-c-comma-operator

namespace meta {
    template <typename... >
    struct vartype_index;

    // found it
    template <typename T, typename... R>
    struct vartype_index<T, T, R...>
        : std::integral_constant<size_t, 0>
    { };

    // still looking
    template <typename T, typename F, typename... R>
    struct vartype_index<T, F, R...>
        : std::integral_constant<size_t, 1 + vartype_index<T, R...>::value>
    { };
}