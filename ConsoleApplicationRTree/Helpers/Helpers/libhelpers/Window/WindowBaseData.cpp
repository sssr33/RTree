#include "WindowBaseData.h"

#if HAVE_WINRT != 1
WindowBaseData::WindowBaseData()
    : handle(NULL)
{
}

void WindowBaseData::PostShutdown() {
    // DestroyWindow doesn't work from another thread.
    // PostMessage works.
    // WindowMessages::Shutdown used to call DestroyWindow from window thread
    // DestroyWindow must(!) be called to clean window resources
    PostMessageW(this->handle, WM_USER, (uint32_t)WindowMessages::Shutdown, 0);
}
#endif // !HAVE_WINRT
