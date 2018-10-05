#include "pch.h"
#include "Rect.h"

#include <limits>
#include <algorithm>

Rect::Rect() {}

Rect::Rect(int left, int top, int right, int bottom)
    : left(left), top(top), right(right), bottom(bottom)
{}

int Rect::Width() const {
    int width = this->right - this->left;
    return width;
}

int Rect::Height() const {
    int height = this->bottom - this->top;
    return height;
}

int Rect::Area() const {
    auto width = this->Width();
    auto height = this->Height();
    auto area = width * height;

    return area;
}

void Rect::Union(const Rect &other) {
    this->left = (std::min)(this->left, other.left);
    this->top = (std::min)(this->top, other.top);

    this->right = (std::max)(this->right, other.right);
    this->bottom = (std::max)(this->bottom, other.bottom);
}

Rect Rect::United(const Rect &other) const {
    auto res = *this;

    res.Union(other);

    return res;
}

Rect Rect::Infinite() {
    Rect r(
        (std::numeric_limits<int>::min)(),
        (std::numeric_limits<int>::min)(),
        (std::numeric_limits<int>::max)(),
        (std::numeric_limits<int>::max)());

    return r;
}

Rect Rect::Degenerated() {
    Rect r(
        (std::numeric_limits<int>::max)(),
        (std::numeric_limits<int>::max)(),
        (std::numeric_limits<int>::min)(),
        (std::numeric_limits<int>::min)());

    return r;
}