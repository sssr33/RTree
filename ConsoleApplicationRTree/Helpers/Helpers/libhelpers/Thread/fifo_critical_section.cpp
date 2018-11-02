#include "fifo_critical_section.h"
#include "..\HSystem.h"

#include <sstream>
#include <thread>

namespace thread {
    fifo_critical_section::fifo_critical_section(DWORD spinCount)
        : critical_section_base<fifo_critical_section>(spinCount),
        queue_head(0), queue_tail(0)
    {
        InitializeConditionVariable(&this->cv);
    }

    fifo_critical_section::~fifo_critical_section() {
    }

    void fifo_critical_section::lock() {
        uint32_t queue_me;

        EnterCriticalSection(this->GetCs());

        // wait the queue. It will wait only for others who locked this <cs>
        queue_me = this->queue_tail++;
        while (queue_me != this->queue_head) {
            SleepConditionVariableCS(&this->cv, this->GetCs(), INFINITE);
        }

        this->own();
        LeaveCriticalSection(this->GetCs());
    }

    void fifo_critical_section::unlock() {
        EnterCriticalSection(this->GetCs());
        this->queue_head++;
        this->unown();
        LeaveCriticalSection(this->GetCs());

        // notify with unlocked cs
        WakeAllConditionVariable(&this->cv);
    }
}