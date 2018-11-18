#pragma once

#include <DirectXMath.h>

class Camera {
public:
    Camera();

    const DirectX::XMFLOAT3 &GetPosition() const;
    DirectX::XMVECTOR GetPositionVec() const;

    void SetPosition(const DirectX::XMFLOAT3 &v);
    void SetPosition(const DirectX::XMVECTOR &v);

    void Move(const DirectX::XMFLOAT3 &v);
    void Move(const DirectX::XMVECTOR &v);

    const DirectX::XMFLOAT3 &GetView() const;
    DirectX::XMVECTOR GetViewVec() const;

    void SetView(const DirectX::XMFLOAT3 &v);
    void SetView(const DirectX::XMVECTOR &v);

    void LookAt(const DirectX::XMFLOAT3 &point);
    void LookAt(const DirectX::XMVECTOR &point);

    void TurnLeft(float rads);
    void TurnRight(float rads);
    void TurnUp(float rads);
    void TurnDown(float rads);

    DirectX::XMMATRIX GetCameraMatrix() const;
    DirectX::XMFLOAT4X4 GetCameraMatrixFlt4x4() const;

private:
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 viewVec;

    DirectX::XMVECTOR GetRightVector() const;
    void RotateView(const DirectX::XMVECTOR &axis, float rads, const DirectX::XMVECTOR *constraintPlaneN = nullptr);
};