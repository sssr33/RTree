#pragma once
#include "../meta/for_each_in_tuple.h"
#include "../meta/vartype_index.h"
#include "../meta/VisBase.h"
#include "../meta/VisWrapper.h"

#include <array>
#include <vector>

namespace containers {
    // Does not supports duplicate types.
    template<class ...Args> class PolyVector {
        struct TypeIdx {
            size_t typeIdx;
            size_t itemIdx;
        };

        struct IVis : public meta::VisBase<Args...> {
            virtual ~IVis() {
            }
        };

        struct WrapperBase {
            virtual void Accept(IVis &visitor, size_t idx) = 0;
            virtual void PopBack() = 0;

        protected:
            ~WrapperBase() {}
        };

        template<class T>
        struct Wrapper : public WrapperBase {
            std::vector<T> vec;

            void Accept(IVis &visitor, size_t idx) override {
                visitor.Visit(this->vec[idx]);
                //this->vec[idx].Accept(visitor);
            }

            void PopBack() override {
                this->vec.pop_back();
            }
        };

        struct Clear_functor {
            template<typename T> void operator () (T&& t) {
                t.vec.clear();
            }
        };

    public:
        PolyVector() {
            this->InitBase();
        }

        PolyVector(const PolyVector &other)
            : pool(other.pool), poolIdx(other.poolIdx)
        {
            this->InitBase();
        }

        PolyVector(PolyVector &&other)
            : PolyVector()
        {
            swap(*this, other);
        }

        PolyVector &operator=(PolyVector other) {
            swap(*this, other);
            return *this;
        }

        friend void swap(PolyVector &a, PolyVector &b) {
            // poolBase must not be swapped because it points to addresses of instance fields
            // instance fields content will be changed
            using std::swap;

            swap(a.pool, b.pool);
            swap(a.poolIdx, b.poolIdx);
        }

        template<class T>
        void Add(T t) {
            TypeIdx cmdIdx;

            cmdIdx.typeIdx = meta::vartype_index<T, Args...>::value;
            cmdIdx.itemIdx = std::get<Wrapper<T>>(this->pool).vec.size();

            std::get<Wrapper<T>>(this->pool).vec.push_back(std::move(t));
            this->poolIdx.push_back(std::move(cmdIdx));
        }

        void Clear() {
            meta::for_each_in_tuple(this->pool, Clear_functor());
            this->poolIdx.clear();
        }

        template<class TVis> void Accept(TVis &visitor) {
            struct VImpl : public meta::VisWrapper<VImpl, TVis, IVis> {
                TVis &impl;

                VImpl(TVis &impl)
                    : impl(impl)
                {
                }

                TVis &GetImpl() {
                    return this->impl;
                }
            };

            VImpl visWrapper(visitor);

            for (auto &i : this->poolIdx) {
                this->poolBase[i.typeIdx]->Accept(visWrapper, i.itemIdx);
            }
        }

        size_t Size() const {
            return this->poolIdx.size();
        }

        void PopBack() {
            auto &i = this->poolIdx.back();
            this->poolBase[i.typeIdx]->PopBack();
            this->poolIdx.pop_back();
        }

    private:
        std::tuple<Wrapper<Args>...> pool;
        std::vector<TypeIdx> poolIdx;
        std::array<WrapperBase*, sizeof...(Args)> poolBase;

        struct InitBase_functor {
            PolyVector *parent;

            template<typename T> void operator () (T&& t) {
                const size_t idx = meta::vartype_index<T, Wrapper<Args>&...>::value;
                this->parent->poolBase[idx] = &t;
            }
        };

        void InitBase() {
            InitBase_functor f;
            f.parent = this;

            meta::for_each_in_tuple(this->pool, f);
        }
    };
}