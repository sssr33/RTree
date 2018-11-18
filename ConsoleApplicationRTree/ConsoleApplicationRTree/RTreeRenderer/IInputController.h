#pragma once

#include <DirectXMath.h>

class IInputController {
public:
    IInputController() {}
    virtual ~IInputController() {}

    virtual DirectX::XMFLOAT3 GetMove() const = 0;
    virtual DirectX::XMFLOAT2 GetTurn() const = 0;

    virtual void Update(float timeDelta) = 0;
};