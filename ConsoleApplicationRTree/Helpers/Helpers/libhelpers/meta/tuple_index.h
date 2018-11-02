#pragma once

#include <tuple>
#include <type_traits>

// https://stackoverflow.com/questions/18063451/get-index-of-a-tuple-elements-type

namespace meta {
    template <class T, class Tuple>
    struct tuple_index;

    // found it
    template <class T, class... Types>
    struct tuple_index<T, std::tuple<T, Types...>>
        : std::integral_constant<size_t, 0>
    {};

    // still looking
    template <class T, class U, class... Types>
    struct tuple_index<T, std::tuple<U, Types...>>
        : std::integral_constant<size_t, 1 + tuple_index<T, std::tuple<Types...>>::value>
    {};
}