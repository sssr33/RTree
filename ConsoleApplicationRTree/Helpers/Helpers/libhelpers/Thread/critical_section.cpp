#include "critical_section.h"
#include "..\HSystem.h"

#include <sstream>
#include <thread>

namespace thread {
    critical_section::critical_section(DWORD spinCount)
        : critical_section_base<critical_section>(spinCount)
    {}

    critical_section::~critical_section() {
    }

    void critical_section::lock() {
        EnterCriticalSection(this->GetCs());
        this->own();
    }

    void critical_section::unlock() {
        this->unown();
        LeaveCriticalSection(this->GetCs());
    }
}