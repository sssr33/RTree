#include "pch.h"
#include "RTreeWindow.h"
#include "InputController.h"

RTreeWindow::RTreeWindow(WindowBaseData &baseData)
    : Window(baseData)
{
}

RTreeRenderer *RTreeWindow::GetRenderer() {
    return this->renderer.get()->get();
}

ProcessMsgResult RTreeWindow::ProcessMsg(uint32_t msg, WPARAM wparam, LPARAM lparam) {
    ProcessMsgResult result(true);

    switch (msg) {
    case WM_CREATE: {
        auto hwnd = this->GetHwnd();
        auto inputController = std::make_unique<InputController>(hwnd);

        this->renderer = std::make_unique<HwndRenderer<RTreeRenderer>>(hwnd, std::move(inputController));
        this->renderer->ContinueRendering();
        break;
    }
    case WM_SIZE: {
        DirectX::XMFLOAT2 size;

        size.x = (float)LOWORD(lparam);
        size.y = (float)HIWORD(lparam);

        this->renderer->Resize(size);
        break;
    }
    default:
        result.handled = false;
        break;
    }

    return result;
}