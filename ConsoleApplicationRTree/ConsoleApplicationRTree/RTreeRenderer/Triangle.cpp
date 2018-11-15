#include "pch.h"
#include "Triangle.h"

float Triangle::DistFromPoint(const DirectX::XMVECTOR &point) const {
    auto v1 = DirectX::XMLoadFloat3(&this->vtx[0].pos);
    auto v2 = DirectX::XMLoadFloat3(&this->vtx[1].pos);
    auto v3 = DirectX::XMLoadFloat3(&this->vtx[2].pos);

    auto a = DirectX::XMVectorSubtract(v1, v2);
    auto b = DirectX::XMVectorSubtract(v3, v2);
    auto n = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(a, b));

    float dist = DirectX::XMVectorGetX(DirectX::XMVector3Dot(n, point));

    return dist;
}