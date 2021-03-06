#pragma once

struct Rect {
    // inclusive
    int left, top;
    // exclusive
    int right, bottom;

    Rect();
    Rect(int left, int top, int right, int bottom);

    bool operator==(const Rect &other) const;
    bool operator!=(const Rect &other) const;

    int Width() const;
    int Height() const;

    int Area() const;

    void Union(const Rect &other);
    Rect United(const Rect &other) const;

    static Rect Infinite();
    static Rect Degenerated();
};