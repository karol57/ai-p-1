#pragma once
#include <cmath>
#include <ostream>

struct vec2d
{
    int x, y;

    double dist2d(vec2d t) const noexcept
    {
        const double dx = x - t.x;
        const double dy = y - t.y;
        return sqrt(static_cast<double>(dx*dx + dy*dy));
    }
};

inline std::ostream& operator<<(std::ostream& s, const vec2d& v)
{
    return s << '(' << v.x << "; " << v.y << ')';
}