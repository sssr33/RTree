#pragma once
#include "Vertex.h"

struct Triangle {
    Vertex vtx[3];

    float DistFromPoint(const DirectX::XMVECTOR &point) const;
};