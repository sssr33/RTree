#pragma once
#include "IInputController.h"

#include <Windows.h>
#include <libhelpers/UniqueHandle.h>

class InputController : public IInputController {
public:
    InputController(HWND winodw);

    DirectX::XMFLOAT3 GetMove() const override;
    DirectX::XMFLOAT2 GetTurn() const override;

    void Update(float timeDelta) override;

private:
    HWND winodw;
    UniqueHandle wndThread;

    DirectX::XMFLOAT3 move;
    DirectX::XMFLOAT2 turn;

    bool pressedW;
    bool pressedA;
    bool pressedS;
    bool pressedD;
    bool pressedEsc;
    bool pressedLMB;
    bool pressedRMB;

    bool mouseControl;
    POINT prevPoint;

    void ProcessKeyboard(bool hasFocus, float timeDelta);
    void ProcessMouseKeys(bool hasFocus, float timeDelta);
    POINT ProcessMouseMoves(bool hasFocus, float timeDelta);

    void ComputeMove(float timeDelta);
    void ComputeTurn(float timeDelta, const POINT &delta);

    POINT GetWndCenterPos() const;

    bool UpdateKeyStateUp(bool hasFocus, int vKey, bool &pressedState);

    template<class UpFn, class DownFn>
    void UpdateKeyState(bool hasFocus, int vKey, bool &pressedState, UpFn upFn, DownFn downFn);
};