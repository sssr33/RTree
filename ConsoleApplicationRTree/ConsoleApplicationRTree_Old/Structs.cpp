#include "stdafx.h"
#include "Structs.h"

#include <cassert>
#include <algorithm>

int& Point::operator[] (const int index) {
    assert(index >= 0 && index < 2);
    return this->m[index];
}

const int& Point::operator[] (const int index) const {
    assert(index >= 0 && index < 2);
    return this->m[index];
}




AABB AABB::CreateEmpty() {
    AABB aabb;

    aabb.min = { 0, 0 };
    aabb.max = { 0, 0 };

    return aabb;
}

AABB AABB::FromMinMax(const Point &min, const Point &max) {
    AABB aabb;

    aabb.min = min;
    aabb.max = max;

    return aabb;
}

AABB AABB::FromPtSize(const Point &pt, const Point &sz) {
    Point min, max, pt2;

    pt2.x = pt.x + sz.x;
    pt2.y = pt.y + sz.y;

    min.x = std::min(pt.x, pt2.x);
    min.y = std::min(pt.y, pt2.y);

    max.x = std::max(pt.x, pt2.x);
    max.y = std::max(pt.y, pt2.y);

    return AABB::FromMinMax(min, max);
}

AABB AABB::Union(const AABB &a, const AABB &b) {
    AABB res;

    res.min.x = std::min(a.min.x, b.min.x);
    res.min.y = std::min(a.min.y, b.min.y);

    res.max.x = std::max(a.max.x, b.max.x);
    res.max.y = std::max(a.max.y, b.max.y);

    return res;
}

int AABB::Width() const {
    return this->max.x - this->min.x;
}

int AABB::Height() const {
    return this->max.y - this->min.y;
}

int AABB::Area() const {
    int width = this->Width();
    int height = this->Height();
    int area = width * height;

    return area;
}

bool AABB::Test(const AABB &other) const {
    auto &a = *this;
    auto &b = other;

    if (a.max[0] <= b.min[0] || a.min[0] >= b.max[0]) return false;
    if (a.max[1] <= b.min[1] || a.min[1] >= b.max[1]) return false;

    return true;
}