#pragma once
#include "critical_section_base.h"

// based on https://stackoverflow.com/questions/12685112/pthreads-thread-starvation-caused-by-quick-re-locking
namespace thread {
    class condition_variable;

    // Better suits tight loops such as game/rendering loop but has more overhead.
    class fifo_critical_section : public critical_section_base<fifo_critical_section> {
    public:
        NO_COPY_MOVE(fifo_critical_section);

        // default value of spinCount - https://msdn.microsoft.com/en-us/library/windows/desktop/ms683477%28v=vs.85%29.aspx
        fifo_critical_section(DWORD spinCount = 4000);
        ~fifo_critical_section();

        void lock();
        void unlock();

    private:
        friend class condition_variable;

        CONDITION_VARIABLE cv;
        uint32_t queue_head, queue_tail;
    };
}