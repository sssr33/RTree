#pragma once

struct Point {
    union {
        int m[2];
        struct {
            int x, y;
        };
    };

    int& operator[] (const int index);
    const int& operator[] (const int index) const;
};

struct AABB {
    Point min; // inclusive
    Point max; // exclusive

    static AABB CreateEmpty();
    static AABB FromMinMax(const Point &min, const Point &max);
    static AABB FromPtSize(const Point &pt, const Point &sz);
    static AABB Union(const AABB &a, const AABB &b);

    int Width() const;
    int Height() const;
    int Area() const;
    bool Test(const AABB &other) const;
};