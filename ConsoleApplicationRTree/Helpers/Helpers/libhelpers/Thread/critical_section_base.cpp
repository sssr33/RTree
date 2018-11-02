#include "critical_section_base.h"
#include "..\HSystem.h"

#include <sstream>
#include <thread>

namespace thread {
    critical_section_base_impl::critical_section_base_impl(DWORD spinCount) {
        InitializeCriticalSectionEx(&this->cs, spinCount, 0);
    }

    critical_section_base_impl::~critical_section_base_impl() {
        DeleteCriticalSection(&this->cs);
    }

    CRITICAL_SECTION *critical_section_base_impl::GetCs() {
        return &this->cs;
    }

    const CRITICAL_SECTION *critical_section_base_impl::GetCs() const {
        return &this->cs;
    }

    void critical_section_base_impl::own() {
#ifdef _DEBUG
        // to prevent recursive locking
        hAssert(!this->owned());

        std::stringstream ss;
        ss << std::this_thread::get_id();

        this->ownerId = ss.str();
#endif // _DEBUG
    }

    void critical_section_base_impl::unown() {
#ifdef _DEBUG
        this->ownerId = std::string();
#endif // _DEBUG
    }

#ifdef _DEBUG
    bool critical_section_base_impl::owned() const {
        std::stringstream ss;
        ss << std::this_thread::get_id();

        bool res = this->ownerId == ss.str();
        return res;
    }
#endif // _DEBUG
}