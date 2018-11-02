#pragma once
#include "seq.h"

namespace meta {
    /*
    <VisBaseGen> generates Visit methods from variadic templates.
    <VisBase> starts generation of Visit methods.
    */

    namespace detail {
        // continue moving to the end. And continue inheritance.
        template<class T, class... VT>
        struct VisBaseGen : public VisBaseGen<VT...> {
            using VisBaseGen<VT...>::Visit;

            virtual void Visit(T&) = 0;
        };

        // last, no args
        template<> struct VisBaseGen<void> {};

        // before last, 1 arg left
        template<class T>
        struct VisBaseGen<T, void> : public VisBaseGen<void> {
            virtual void Visit(T&) = 0;
        };
    }

    template<class... VT>
    struct VisBase : public detail::VisBaseGen<VT..., void> {
        // Type sequence of visitor
        typedef type_seq<VT...> seq;

        /*
        <VisBase> appends <void> to the end of type pack in order to mark the end.
        <void> also lets <VisBaseGen> know when to stop <Visit> generation.
        */
    };
}