#pragma once
#include "seq.h"

#include <tuple>

// https://stackoverflow.com/questions/16387354/template-tuple-calling-a-function-on-each-element

namespace meta {
    namespace detail {
        template<typename T, typename F, int... Is>
        void for_each_in_tuple(T&& t, F f, int_seq<Is...>) {
            auto l = { (f(std::get<Is>(t)), 0)... };
        }
    }

    template<typename... Ts, typename F>
    void for_each_in_tuple(std::tuple<Ts...> & t, F f) {
        detail::for_each_in_tuple(t, f, gen_seq<sizeof...(Ts)>());
    }
}