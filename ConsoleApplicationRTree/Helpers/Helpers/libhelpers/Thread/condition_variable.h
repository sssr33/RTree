#pragma once
#include "..\Macros.h"

#include <Windows.h>

namespace thread {
    class critical_section;
    class fifo_critical_section;

    class condition_variable {
    public:
        NO_COPY_MOVE(condition_variable);

        condition_variable();
        ~condition_variable();

        void wait(critical_section &cs, DWORD milliseconds = INFINITE, bool *timeout = nullptr);
        void wait(fifo_critical_section &cs, DWORD milliseconds = INFINITE, bool *timeout = nullptr);

        void notify();
        void notify_all();

    private:
        CONDITION_VARIABLE cv;
    };
}