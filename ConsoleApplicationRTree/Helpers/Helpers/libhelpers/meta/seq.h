#pragma once

// https://stackoverflow.com/questions/16387354/template-tuple-calling-a-function-on-each-element

namespace meta {
    // small helper to get vartype pack inside meta-programs
    template<class... T> struct type_seq {};

    // small helper to get int pack inside meta-programs
    template<int... Is> struct int_seq { };

    // expanding sequence from N to 0
    // each iteration will use this specialization until N == 0 then it will use gen_seq<0, Is...>
    template<int N, int... Is>
    struct gen_seq : gen_seq<N - 1, N - 1, Is...> { };

    // got whole sequence {0, 0, 1, 2, 3...}
    // now can slice gen_seq as seq to get sequence without extra 0
    template<int... Is>
    struct gen_seq <0, Is...> : int_seq<Is...> { };
}