#pragma once
#include "..\config.h"

#if HAVE_WINRT != 1
#include <thread>
#include <string>
#include <windows.h>

enum class WindowMessages : uint32_t {
    Shutdown // used to call DestroyWindow from window thread
};

class WindowBaseData {
public:
    HWND handle;
    std::wstring className;
    std::thread wndThread;

    WindowBaseData();

    void PostShutdown();
};
#endif // !HAVE_WINRT
