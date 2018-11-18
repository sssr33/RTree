#include "pch.h"
#include "Camera.h"

Camera::Camera()
    : position(0.f, 0.f, 0.f), viewVec(0.f, 0.f, 1.f)
{}

const DirectX::XMFLOAT3 &Camera::GetPosition() const {
    return this->position;
}

DirectX::XMVECTOR Camera::GetPositionVec() const {
    auto pos = DirectX::XMLoadFloat3(&this->position);

    pos = DirectX::XMVectorSetW(pos, 1.f);
    return pos;
}

void Camera::SetPosition(const DirectX::XMFLOAT3 &v) {
    this->position = v;
}

void Camera::SetPosition(const DirectX::XMVECTOR &v) {
    DirectX::XMStoreFloat3(&this->position, v);
}

void Camera::Move(const DirectX::XMFLOAT3 &v) {
    auto tmpMove = DirectX::XMLoadFloat3(&v);
    this->Move(tmpMove);
}

void Camera::Move(const DirectX::XMVECTOR &v) {
    auto tmpPos = DirectX::XMLoadFloat3(&this->position);

    DirectX::XMVECTOR right = this->GetRightVector();
    DirectX::XMVECTOR up = DirectX::g_XMIdentityR1;
    DirectX::XMVECTOR fwd = DirectX::XMLoadFloat3(&this->viewVec);

    right = DirectX::XMVectorScale(right, DirectX::XMVectorGetX(v));
    up = DirectX::XMVectorScale(up, DirectX::XMVectorGetY(v));
    fwd = DirectX::XMVectorScale(fwd, DirectX::XMVectorGetZ(v));

    tmpPos = DirectX::XMVectorAdd(tmpPos, right);
    tmpPos = DirectX::XMVectorAdd(tmpPos, up);
    tmpPos = DirectX::XMVectorAdd(tmpPos, fwd);

    DirectX::XMStoreFloat3(&this->position, tmpPos);
}

const DirectX::XMFLOAT3 &Camera::GetView() const {
    return this->viewVec;
}

DirectX::XMVECTOR Camera::GetViewVec() const {
    auto view = DirectX::XMLoadFloat3(&this->viewVec);

    view = DirectX::XMVectorSetW(view, 0.f);
    return view;
}

void Camera::SetView(const DirectX::XMFLOAT3 &v) {
    auto view = DirectX::XMLoadFloat3(&this->viewVec);
    this->SetView(view);
}

void Camera::SetView(const DirectX::XMVECTOR &v) {
    auto viewVec = DirectX::XMVector3Normalize(v);
    DirectX::XMStoreFloat3(&this->viewVec, viewVec);
}

void Camera::LookAt(const DirectX::XMFLOAT3 &point) {
    auto pt = DirectX::XMLoadFloat3(&point);
    this->LookAt(pt);
}

void Camera::LookAt(const DirectX::XMVECTOR &point) {
    auto pos = DirectX::XMLoadFloat3(&this->position);
    auto viewVec = DirectX::XMVectorSubtract(point, pos);

    this->SetView(viewVec);
}

void Camera::TurnLeft(float rads) {
    this->TurnRight(-rads);
}

void Camera::TurnRight(float rads) {
    this->RotateView(DirectX::g_XMIdentityR1, rads);
}

void Camera::TurnUp(float rads) {
    this->TurnDown(-rads);
}

void Camera::TurnDown(float rads) {
    auto right = this->GetRightVector();

    auto viewConstraint = DirectX::XMVector3Cross(right, DirectX::g_XMIdentityR1);
    viewConstraint = DirectX::XMVector3Normalize(viewConstraint);

    this->RotateView(right, rads, &viewConstraint);
}

DirectX::XMMATRIX Camera::GetCameraMatrix() const {
    auto pos = this->GetPositionVec();
    auto view = this->GetViewVec();
    auto up = DirectX::g_XMIdentityR1;

    auto matrix = DirectX::XMMatrixLookToLH(pos, view, up);

    return matrix;
}

DirectX::XMFLOAT4X4 Camera::GetCameraMatrixFlt4x4() const {
    DirectX::XMFLOAT4X4 matrix4x4;
    auto matrix = this->GetCameraMatrix();

    DirectX::XMStoreFloat4x4(&matrix4x4, matrix);

    return matrix4x4;
}

DirectX::XMVECTOR Camera::GetRightVector() const {
    auto up = DirectX::g_XMIdentityR1;
    auto view = DirectX::XMLoadFloat3(&this->viewVec);

    auto right = DirectX::XMVector3Cross(up, view);
    right = DirectX::XMVector3Normalize(right);

    return right;
}

void Camera::RotateView(const DirectX::XMVECTOR &axis, float rads, const DirectX::XMVECTOR *constraintPlaneN) {
    auto view = DirectX::XMLoadFloat3(&this->viewVec);
    auto rotQuat = DirectX::XMQuaternionRotationAxis(axis, rads);

    auto newView = DirectX::XMVector3Rotate(view, rotQuat);

    if (constraintPlaneN) {
        float afterRotation = DirectX::XMVectorGetX(DirectX::XMVector3Dot(newView, *constraintPlaneN));
        float beforeRotation = DirectX::XMVectorGetX(DirectX::XMVector3Dot(view, *constraintPlaneN));

        bool afterLessZero = afterRotation < 0.f;
        bool beforeLessZero = beforeRotation < 0.f;

        if (afterLessZero != beforeLessZero) {
            float dest = 0.001f;

            if (beforeLessZero) {
                dest = -dest;
            }

            float one = afterRotation - beforeRotation;
            float limit = dest - beforeRotation;

            float t = limit / one;
            rads *= t;

            rotQuat = DirectX::XMQuaternionRotationAxis(axis, rads);
            newView = DirectX::XMVector3Rotate(view, rotQuat);
        }
    }

    this->SetView(newView);
}