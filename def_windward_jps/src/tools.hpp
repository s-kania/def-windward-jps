#pragma once

#include <math.h>
#include <stdlib.h>

#include "grid.hpp"

namespace Tool
{
    inline int manhattan_int(const Location& a, const Location& b)
    {
        return 1000 * ((a.x > b.x ? a.x - b.x : b.x - a.x) + (a.y > b.y ? a.y - b.y : b.y - a.y));
    }

    inline int octile_int(const Location& a, const Location& b)
    {
        const int dx = (a.x > b.x) ? (a.x - b.x) : (b.x - a.x);
        const int dy = (a.y > b.y) ? (a.y - b.y) : (b.y - a.y);
        const int min_val = (dx < dy) ? dx : dy;
        const int max_val = (dx > dy) ? dx : dy;
        return 1000 * max_val + 414 * min_val;
    }

    inline double manhattan(const Location& a, const Location& b)
    {
        return manhattan_int(a, b) / 1000.0;
    }

    inline double euclidean(const Location& a, const Location& b)
    {
        const int dx = a.x - b.x;
        const int dy = a.y - b.y;
        return sqrt((double)(dx * dx + dy * dy));
    }

    inline double octile(const Location& a, const Location& b)
    {
        return octile_int(a, b) / 1000.0;
    }
}
