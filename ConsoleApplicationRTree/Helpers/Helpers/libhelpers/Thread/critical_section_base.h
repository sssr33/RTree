#pragma once
#include "..\Macros.h"

#include <Windows.h>
#include <chrono>
#include <memory>
#include <string>
#include <algorithm>

namespace thread {
    class critical_section_base_impl {
    public:
        critical_section_base_impl(DWORD spinCount);
        ~critical_section_base_impl();

        CRITICAL_SECTION *GetCs();
        const CRITICAL_SECTION *GetCs() const;

        void own();
        void unown();

#ifdef _DEBUG
        bool owned() const;
#endif // _DEBUG

    private:
        CRITICAL_SECTION cs;

#ifdef _DEBUG
        std::string ownerId;
#endif // _DEBUG
    };

    template<class Base>
    class critical_section_base {
    public:
        NO_COPY_MOVE(critical_section_base);

        // default value of spinCount - https://msdn.microsoft.com/en-us/library/windows/desktop/ms683477%28v=vs.85%29.aspx
        critical_section_base(DWORD spinCount = 4000)
            : baseImpl(spinCount)
        {}

        ~critical_section_base() {}

        void lock() {
            this->impl()->lock();
        }

        void unlock() {
            this->impl()->unlock();
        }

#ifdef _DEBUG
        bool owned() const {
            return this->baseImpl.owned();
        }
#endif // _DEBUG

    protected:
        // must be called in implementation of lock
        void own() {
            this->baseImpl.own();
        }

        // must be called in implementation of unlock
        void unown() {
            this->baseImpl.unown();
        }

        CRITICAL_SECTION *GetCs() {
            return this->baseImpl.GetCs();
        }

        const CRITICAL_SECTION *GetCs() const {
            return this->baseImpl.GetCs();
        }

    private:
        critical_section_base_impl baseImpl;

        Base *impl() {
            return static_cast<Base*>(this);
        }

    public:
        class scoped_lock {
        public:
            scoped_lock()
                : cs(nullptr)
            {}

            scoped_lock(const scoped_lock &other) = delete;

            scoped_lock(Base &cs)
                : cs(&cs)
            {
                this->cs->lock();
            }

            scoped_lock(std::unique_ptr<Base> &cs)
                : cs(cs.get())
            {
                this->cs->lock();
            }

            scoped_lock(std::shared_ptr<Base> &cs)
                : cs(cs.get())
            {
                this->cs->lock();
            }

            scoped_lock(scoped_lock &&other)
                : scoped_lock()
            {
                swap(*this, other);
            }

            ~scoped_lock() {
                if (this->cs) {
                    this->cs->unlock();
                }
            }

            scoped_lock &operator=(scoped_lock other) {
                swap(*this, other);
                return *this;
            }

            friend void swap(scoped_lock &first, scoped_lock &second) {
                // enable ADL (not necessary in our case, but good practice)
                using std::swap;

                // by swapping the members of two objects,
                // the two objects are effectively swapped
                swap(first.cs, second.cs);
            }

        private:
            Base *cs;
        };
    };

    template<class T> typename T::scoped_lock scoped_lock(T &cs) {
        return typename T::scoped_lock(cs);
    }

    template<class T> typename T::scoped_lock scoped_lock(std::unique_ptr<T> &cs) {
        return typename T::scoped_lock(cs);
    }

    template<class T> typename T::scoped_lock scoped_lock(std::shared_ptr<T> &cs) {
        return typename T::scoped_lock(cs);
    }
}