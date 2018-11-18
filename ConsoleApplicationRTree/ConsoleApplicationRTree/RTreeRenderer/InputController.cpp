#include "pch.h"
#include "InputController.h"

#include <libhelpers/HSystem.h>
#include <libhelpers/UnkGenericAction.h>

InputController::InputController(HWND winodw)
    : winodw(winodw)
    , move(0.f, 0.f, 0.f), turn(0.f, 0.f)
    , pressedW(false), pressedA(false)
    , pressedS(false), pressedD(false)
    , pressedEsc(false)
    , pressedLMB(false), pressedRMB(false)
    , mouseControl(false), prevPoint({})
{
    auto wndThreadId = GetWindowThreadProcessId(this->winodw, nullptr);
    this->wndThread = UniqueHandle(OpenThread(THREAD_ALL_ACCESS, FALSE, wndThreadId));
}

DirectX::XMFLOAT3 InputController::GetMove() const {
    return this->move;
}

DirectX::XMFLOAT2 InputController::GetTurn() const {
    return this->turn;
}

void InputController::Update(float timeDelta) {
    bool hasFocus = (this->winodw == GetForegroundWindow());

    this->ProcessKeyboard(hasFocus, timeDelta);
    this->ProcessMouseKeys(hasFocus, timeDelta);
    auto delta = this->ProcessMouseMoves(hasFocus, timeDelta);

    this->ComputeMove(timeDelta);
    this->ComputeTurn(timeDelta, delta);
}

void InputController::ProcessKeyboard(bool hasFocus, float timeDelta) {
    this->UpdateKeyStateUp(hasFocus, 0x57, this->pressedW);
    this->UpdateKeyStateUp(hasFocus, 0x41, this->pressedA);
    this->UpdateKeyStateUp(hasFocus, 0x53, this->pressedS);
    this->UpdateKeyStateUp(hasFocus, 0x44, this->pressedD);

    if (this->UpdateKeyStateUp(hasFocus, VK_ESCAPE, this->pressedEsc)) {
        this->mouseControl = !this->mouseControl;

            if (this->mouseControl) {
                QueueUserAPC([](ULONG_PTR Parameter) {
                    ShowCursor(FALSE);
                }, this->wndThread.get(), 0);

                    this->prevPoint = this->GetWndCenterPos();

                    SetCursorPos(this->prevPoint.x, this->prevPoint.y);
            }
            else {
                QueueUserAPC([](ULONG_PTR Parameter) {
                    ShowCursor(TRUE);
                }, this->wndThread.get(), 0);
            }
    }
}

void InputController::ProcessMouseKeys(bool hasFocus, float timeDelta) {
    this->UpdateKeyStateUp(hasFocus, VK_LBUTTON, this->pressedLMB);
    this->UpdateKeyStateUp(hasFocus, VK_RBUTTON, this->pressedRMB);
}

POINT InputController::ProcessMouseMoves(bool hasFocus, float timeDelta) {
    POINT pos;
    POINT delta = {};

    if (!GetCursorPos(&pos)) {
        return delta;
    }

    delta.x = pos.x - this->prevPoint.x;
    delta.y = pos.y - this->prevPoint.y;

    if (this->mouseControl) {
        pos = this->GetWndCenterPos();
    }

    this->prevPoint = pos;

    if (this->mouseControl) {
        SetCursorPos(this->prevPoint.x, this->prevPoint.y);
    }

    return delta;
}

void InputController::ComputeMove(float timeDelta) {
    this->move = DirectX::XMFLOAT3(0.f, 0.f, 0.f);

    const float speedX = 3.0f;
    const float speedZ = 3.0f;

    if (!this->mouseControl) {
        return;
    }

    if (this->pressedW) {
        this->move.z += speedZ * timeDelta;
    }

    if (this->pressedA) {
        this->move.x += -speedX * timeDelta;
    }

    if (this->pressedS) {
        this->move.z += -speedZ * timeDelta;
    }

    if (this->pressedD) {
        this->move.x += speedX * timeDelta;
    }
}

void InputController::ComputeTurn(float timeDelta, const POINT &delta) {
    this->turn = DirectX::XMFLOAT2(0.f, 0.f);

    if (!this->mouseControl) {
        return;
    }

    const float speedX = DirectX::XMConvertToRadians(0.5f);
    const float speedY = DirectX::XMConvertToRadians(0.5f);

    this->turn.x = -delta.x * speedX;
    this->turn.y = -delta.y * speedY;
}

POINT InputController::GetWndCenterPos() const {
    POINT pt = {};
    RECT clientRect;

    if (!GetClientRect(this->winodw, &clientRect)) {
        return this->prevPoint;
    }

    pt.x = clientRect.left + (clientRect.right - clientRect.left) / 2;
    pt.y = clientRect.top + (clientRect.bottom - clientRect.top) / 2;

    if (!ClientToScreen(this->winodw, &pt)) {
        return this->prevPoint;
    }

    return pt;
}

bool InputController::UpdateKeyStateUp(bool hasFocus, int vKey, bool &pressedState) {
    bool up = false;

    this->UpdateKeyState(hasFocus, vKey, pressedState, [&]() { up = true; }, []() {});

    return up;
}

template<class UpFn, class DownFn>
void InputController::UpdateKeyState(bool hasFocus, int vKey, bool &pressedState, UpFn upFn, DownFn downFn) {
    bool tmpPressed = false;

    if (hasFocus) {
        auto state = GetAsyncKeyState(vKey);

        if ((1 << 15) & state) {
            tmpPressed = true;
        }
    }

    if (!pressedState && tmpPressed) {
        downFn();
    }
    else if (pressedState && !tmpPressed) {
        upFn();
    }

    pressedState = tmpPressed;
}