#include "Window.h"

#if HAVE_WINRT != 1
#include <libhelpers\HSystem.h>

ProcessMsgResult::ProcessMsgResult()
    : lresult(0), handled(false)
{
}

ProcessMsgResult::ProcessMsgResult(bool handled)
    : lresult(0), handled(handled)
{
}

Window::Window(WindowBaseData &baseData)
    : baseData(baseData)
{
}

Window::~Window() {
}

DirectX::XMUINT2 Window::GetSize() const {
    RECT rect;
    DirectX::XMUINT2 size;

    if (GetClientRect(this->baseData.handle, &rect)) {
        size.x = rect.right - rect.left;
        size.y = rect.bottom - rect.top;
    }
    else {
        size.x = size.y = 1;
    }

    return size;
}

void Window::ProcessMessages() {
    MSG msg;

    while (PeekMessageW(&msg, this->baseData.handle, 0, 0, PM_REMOVE)) {
        DispatchMessageW(&msg);
    }
}

HWND Window::GetHwnd() const {
    return this->baseData.handle;
}

const std::wstring &Window::GetWndClassName() const {
    return this->baseData.className;
}

void Window::PostShutdown() {
    this->baseData.PostShutdown();
}
#endif // !HAVE_WINRT
