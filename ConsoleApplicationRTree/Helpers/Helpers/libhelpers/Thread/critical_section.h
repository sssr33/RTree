#pragma once
#include "critical_section_base.h"

namespace thread {
    class condition_variable;

    // Critical section for common use. For tight loops use fifo_critical_section
    class critical_section : public critical_section_base<critical_section> {
    public:
        NO_COPY_MOVE(critical_section);

        // default value of spinCount - https://msdn.microsoft.com/en-us/library/windows/desktop/ms683477%28v=vs.85%29.aspx
        critical_section(DWORD spinCount = 4000);
        ~critical_section();

        void lock();
        void unlock();

    private:
        friend class condition_variable;
    };
}