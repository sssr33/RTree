#pragma once
#include "..\config.h"
#if HAVE_WINRT != 1
#include "WindowMain.h"

template<class T>
class WindowContainer {
public:
    WindowContainer(const std::wstring &windowTitle)
        : window(baseData), winMain(baseData, window, windowTitle)
    {
    }

    T *operator->() {
        return &this->window;
    }

    const T *operator->() const {
        return &this->window;
    }

    void WaitForClose() {
        this->winMain.WaitForClose();
    }

private:
    WindowBaseData baseData;
    T window;
    WindowMain winMain;
};
#endif // !HAVE_WINRT
