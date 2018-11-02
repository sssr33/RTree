#pragma once
#include "seq.h"

namespace meta {
    /*
    <has_vis_method> detects whether <T> has method <visit> with argument of type <A&>
    <VisWrapperGen> implements Visi methods and redirects calls to <Impl> if it has <visit> method with appropriate signature
    <VisWrapper1> unwraps type pack for 1 argument and starts <VisWrapperGen> and helps with overloading resolution by using VisBase::Visit;
    <VisWrapper> accepts required types for wrapper generation and adds end marker as void,void
    */

    namespace detail {
        // https://stackoverflow.com/questions/87372/check-if-a-class-has-a-member-function-of-a-given-signature
        template<class T, class A>
        class has_vis_method {
            template<class U, void(U::*)(A&)> struct SFINAE {};
            template<class U> static char Test(SFINAE<U, &U::visit>*);
            template<class U> static double Test(...);

        public:
            static const bool has = sizeof(Test<T>(nullptr)) == sizeof(char);
        };

        // https://en.wikibooks.org/wiki/More_C++_Idioms/Member_Detector
        template<typename T>
        class has_seq_member {
            struct Fallback { int seq; }; // add member name "seq"
            struct Derived : T, Fallback { };

            template<typename U, U> struct SFINAE;

            // this overload can be selected when there is no ambiguity, i.e. <U> has only 1 <seq> member.
            // U can have only 1 <seq> member when <T> doesn't have <seq> member
            template<typename U>
            static char func(SFINAE<int Fallback::*, &U::seq> *);

            // this overload can be selected when there IS ambiguity, i.e. <U> has more than 1 <seq> members.
            // U can have more than 1 <seq> member when <T> has <seq> member
            template<typename U>
            static double func(...);

        public:
            static const bool has = sizeof(func<Derived>(nullptr)) == sizeof(double);
        };

        // void& is considered as non valid argument type
        template<class T>
        struct has_vis_method<T, void> {
            static const bool has = false;
        };

        // <Impl> has appropriate <visit> method so create implemetation and redirect call. And continue inheritance. Pass <VisBaseImpl> until the end.
        template<class Base, class Impl, class VisBaseImpl, bool has, class T, class TN, typename... VT>
        struct VisWrapperGen : public VisWrapperGen<Base, Impl, VisBaseImpl, has_vis_method<Impl, TN>::has, TN, VT...> {
            void Visit(T &v) override {
                Impl &impl = static_cast<Base*>(this)->GetImpl();
                impl.visit(v);
            }
        };

        // <Impl> doesn't have <visit> method with appropriate signature so implement <Visit> as empty method. And continue inheritance. Pass <VisBaseImpl> until the end.
        template<class Base, class Impl, class VisBaseImpl, class T, class TN, typename... VT>
        struct VisWrapperGen<Base, Impl, VisBaseImpl, false, T, TN, VT...> : public VisWrapperGen<Base, Impl, VisBaseImpl, has_vis_method<Impl, TN>::has, TN, VT...> {
            void Visit(T &v) override {
            }
        };

        // The end marker (void,void) has been reached. End inheritance chain by inheriting from <VisBaseImpl>.
        template<class Base, class Impl, class VisBaseImpl>
        struct VisWrapperGen<Base, Impl, VisBaseImpl, false, void, void> : public VisBaseImpl {
        };

        // Gets 1st type and starts generation of Wrapper implementation and inspects hasSeqMember
        template<class Base, class Impl, class VisBaseImpl, bool hasSeqMember, typename T, typename... VT>
        struct VisWrapper1 : public VisWrapperGen<Base, Impl, VisBaseImpl, has_vis_method<Impl, T>::has, T, VT...> {
            using VisBaseImpl::Visit;
        };

        template<class Base, class Impl, class VisBaseImpl, class Seq, typename T, typename... VT>
        struct VisWrapper1Seq {
            // no impl...
        };

        /*<SVT> has been extracted from <type_seq>.
        Inherit again from <VisWrapper1> but now do not analyze hasSeqMember because <SVT...> is what was needed, append end marker(void,void) again.

        Here we will have such inheritance tree:
        VisWrapper<... , VisBaseImpl, ...>
            VisWrapper1<... , VisBaseImpl, hasSeqMember == true, void, void>
                VisWrapper1Seq<... , VisBaseImpl, type_seq<SVT...>, void, void>
                    VisWrapper1<..., VisBaseImpl, hasSeqMember == false, SVT..., void, void>
                        VisWrapperGen<....>

        VisWrapper1->VisWrapper1Seq->VisWrapper1 is perfectly valid here because it will not create infinite cycle and will use VisWrapperGen as next parent.
        */
        template<class Base, class Impl, class VisBaseImpl, typename... SVT>
        struct VisWrapper1Seq<Base, Impl, VisBaseImpl, type_seq<SVT...>, void, void> : public VisWrapper1<Base, Impl, VisBaseImpl, false, SVT..., void, void> {
        };

        // <hasSeqMember> == true, T == void, VT... == void
        // try to extract type of <seq> and its typelist through <VisWrapper1Seq> specializations
        template<class Base, class Impl, class VisBaseImpl>
        struct VisWrapper1<Base, Impl, VisBaseImpl, true, void, void> : public VisWrapper1Seq<Base, Impl, VisBaseImpl, typename VisBaseImpl::seq, void, void> {
            using VisBaseImpl::Visit;
        };
    }

    // Pass <VT> pack to <VisWrapper1> and append end marker (T = void, TN = void for VisWrapperGen)
    // VisBaseImpl must inherit from VisBase.h/VisBase
    template<class Base, class Impl, class VisBaseImpl, typename... VT>
    struct VisWrapper : public detail::VisWrapper1<Base, Impl, VisBaseImpl, detail::has_seq_member<VisBaseImpl>::has, VT..., void, void> {
    };
}