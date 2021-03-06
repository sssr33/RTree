#include "WindowMain.h"

#if HAVE_WINRT != 1
#include <libhelpers\HSystem.h>

WindowMain::WindowMain(WindowBaseData &baseData, Window &window, const std::wstring &windowTitle)
    : baseData(baseData), window(window), destroyed(false)
{
    WindowInitData initData;
    auto initFuture = initData.InitPromise.get_future();

    initData.windowTitle = windowTitle;

    this->baseData.wndThread = std::thread([this, &initData]() {
        this->WndThreadMain(&initData);
    });

    initFuture.get();
}

WindowMain::~WindowMain() {

    {
        thread::critical_section::scoped_lock lk(this->destroyCs);

        if (!this->destroyed) {
            // DestroyWindow doesn't work from another thread.
            // PostMessage works.
            // WindowMessages::Shutdown used to call DestroyWindow from window thread
            // DestroyWindow must(!) be called to clean window resources
            this->baseData.PostShutdown();
        }
    }

    if (this->baseData.wndThread.joinable()) {
        this->baseData.wndThread.join();
    }

    WindowMain::RemoveThisMap(this->baseData.handle, this->baseData.className);
}

void WindowMain::WaitForClose() {
    thread::critical_section::scoped_lock lk(this->destroyCs);

    while (!this->destroyed) {
        this->destroyCv.wait(this->destroyCs);
    }
}

void WindowMain::Show() {
    auto tmp = ShowWindow(this->baseData.handle, SW_SHOWDEFAULT);
    int top = 234;
}

void WindowMain::WndThreadMain(WindowInitData *initData) {
    bool initFailed = false;
    try {
        WNDCLASSW wndclass;
        auto hinst = GetModuleHandleW(NULL);
        this->baseData.className = L"WindowMain" + std::to_wstring(WindowMain::nextWndId++);

        wndclass.style = CS_DBLCLKS;
        wndclass.lpfnWndProc = WindowMain::WndProcTmp;
        wndclass.cbClsExtra = 0;
        wndclass.cbWndExtra = 0;
        wndclass.hInstance = hinst;
        wndclass.hIcon = NULL;
        wndclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wndclass.hbrBackground = HBRUSH(COLOR_WINDOW + 1);
        wndclass.lpszMenuName = NULL;
        wndclass.lpszClassName = this->baseData.className.data();

        if (!RegisterClassW(&wndclass)) {
            H::System::ThrowIfFailed(E_FAIL);
        }

        WindowMain::AddThisMap2(this->baseData.className, this);

        this->baseData.handle = CreateWindowW(
            this->baseData.className.data(),
            initData->windowTitle.c_str(),
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            NULL,
            NULL,
            hinst,
            NULL);

        if (!this->baseData.handle) {
            DWORD err = GetLastError();
            HRESULT hr = HRESULT_FROM_WIN32(err);

            H::System::ThrowIfFailed(hr);
        }

        WindowMain::AddThisMap(this->baseData.handle, this);

        this->Show();

        initData->InitPromise.set_value(true);
    }
    catch (...) {
        initData->InitPromise.set_exception(std::current_exception());
        initFailed = true;
    }
    initData = nullptr; // Must not be used after InitPromise.set_value/set_exception

    if (!initFailed) {
        MSG msg;
        bool quit = false;

        // https://weblogs.asp.net/kennykerr/parallel-programming-with-c-part-2-asynchronous-procedure-calls-and-window-messages
        while (!quit) {
            DWORD result = MsgWaitForMultipleObjectsEx(0, nullptr, INFINITE, QS_ALLINPUT, MWMO_ALERTABLE | MWMO_INPUTAVAILABLE);

            if (WAIT_FAILED == result) {
                auto msg = std::string() + "MsgWaitForMultipleObjectsEx failed " + std::to_string(GetLastError());
                H::System::DebugOutput(msg);
                continue;
            }

            assert(WAIT_IO_COMPLETION == result || WAIT_OBJECT_0 == result);

            if (WAIT_OBJECT_0 == result) {
                while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                    TranslateMessage(&msg);
                    DispatchMessageW(&msg);

                    if (WM_QUIT == msg.message) {
                        quit = true;
                    }
                }
            }
        }
    }
}

LRESULT WindowMain::WndProc(HWND hwnd, uint32_t msg, WPARAM wparam, LPARAM lparam) {
    auto procMsgRes = this->window.ProcessMsg(msg, wparam, lparam);

    if (!procMsgRes.handled) {
        switch (msg) {
        case WM_USER: {
            auto msg = (WindowMessages)wparam;
            switch (msg) {
            case WindowMessages::Shutdown:
                DestroyWindow(this->baseData.handle);
                break;
            default:
                break;
            }
            break;
        }
        case WM_DESTROY: {
            {
                thread::critical_section::scoped_lock lk(this->destroyCs);
                this->destroyed = true;
            }
            this->destroyCv.notify_all();

            PostQuitMessage(0);
            break;
        }
        default:
            // using hwnd from parameter because this->baseData.handle is null while initializing
            return DefWindowProc(hwnd, msg, wparam, lparam);
        }
    }
    else {
        return procMsgRes.lresult;
    }

    return 0L;
}

std::atomic_uint64_t WindowMain::nextWndId(0);
thread::critical_section WindowMain::thisMapCs;
std::map<HWND, WindowMain *> WindowMain::thisMap;
std::map<std::wstring, WindowMain *> WindowMain::thisMap2;

LRESULT CALLBACK WindowMain::WndProcTmp(HWND h, UINT msg, WPARAM wparam, LPARAM lparam) {
    WindowMain *_this = nullptr;

    {
        thread::critical_section::scoped_lock lk(WindowMain::thisMapCs);

        auto findH = WindowMain::thisMap.find(h);

        if (findH != WindowMain::thisMap.end()) {
            _this = findH->second;

            // should be alway true
            H::System::Assert(h == _this->baseData.handle);
        }
        else {
            auto name = WindowMain::GetWindowClassName(h);
            auto findName = WindowMain::thisMap2.find(name);

            if (findName != WindowMain::thisMap2.end()) {
                _this = findName->second;

                // initialize handle for window now 
                // to allow window class use it
                WindowMain::thisMap[h] = _this;
                _this->baseData.handle = h;
            }
        }
    }

    H::System::Assert(_this != nullptr);

    return _this->WndProc(h, msg, wparam, lparam);
}

void WindowMain::AddThisMap(HWND h, WindowMain *_this) {
    thread::critical_section::scoped_lock lk(WindowMain::thisMapCs);

    auto find = WindowMain::thisMap.find(h);

    if (find == WindowMain::thisMap.end()) {
        WindowMain::thisMap[h] = _this;
    }
    else {
        H::System::Assert(_this == find->second);
    }
}

void WindowMain::RemoveThisMap(HWND h, const std::wstring &name) {
    thread::critical_section::scoped_lock lk(WindowMain::thisMapCs);
    WindowMain::thisMap.erase(h);
    WindowMain::thisMap2.erase(name);
}

void WindowMain::AddThisMap2(const std::wstring &name, WindowMain *_this) {
    thread::critical_section::scoped_lock lk(WindowMain::thisMapCs);
    WindowMain::thisMap2[name] = _this;
}

std::wstring WindowMain::GetWindowClassName(HWND h) {
    const size_t bufSize = 64;
    wchar_t buf[bufSize];
    std::wstring name;

    int res = GetClassNameW(h, buf, bufSize);

    if (res > 0) {
        H::System::Assert(res < bufSize);
        name = std::wstring(buf, buf + res);
    }

    return name;
}
#endif // !HAVE_WINRT
