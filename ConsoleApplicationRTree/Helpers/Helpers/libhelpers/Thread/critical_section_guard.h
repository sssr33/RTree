#pragma once
#include "critical_section.h"

namespace thread {
    template<class T>
    class critical_section_guard {
    public:
        NO_COPY_MOVE(critical_section_guard);

        struct SpinCount {
            DWORD val;
        };

        template<class... Args>
        critical_section_guard(SpinCount spinCount, Args&& ...args)
            : cs(spinCount.val), obj(std::forward<Args>...)
        {}

        template<class... Args>
        critical_section_guard(Args&& ...args)
            : obj(std::forward<Args>...)
        {}

        T *get() {
            return &this->obj;
        }

        const T *get() const {
            return &this->obj;
        }

        critical_section::scoped_lock lock_scoped() {
            return critical_section::scoped_lock(this->cs);
        }

        void lock() {
            this->cs.lock();
        }

        void unlock() {
            this->cs.unlock();
        }

        const critical_section &get_cs() const {
            return this->cs;
        }

        critical_section &get_cs() {
            return this->cs;
        }

    private:
        critical_section cs;
        T obj;
    };
}