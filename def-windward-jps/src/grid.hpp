#pragma once

#include <stddef.h>

struct Location
{
    int x;
    int y;
    inline Location direction() const {
        Location result;
        result.x = 0;
        result.y = 0;
        if(x > 0) { result.x = 1; }
        else if(x < 0) { result.x = -1; }
        else { result.x = 0; }
        if(y > 0) { result.y = 1; }
        else if(y < 0) { result.y = -1; }
        else { result.y = 0; }
        return result;
    }
};

inline Location make_location(int x, int y) {
    Location result;
    result.x = x;
    result.y = y;
    return result;
}

inline bool operator==(const Location& a, const Location& b) {
    return a.x == b.x && a.y == b.y;
}
inline bool operator!=(const Location& a, const Location& b) {
    return a.x != b.x || a.y != b.y;
}
bool operator<(const Location& a, const Location& b);

inline Location operator+(const Location& a, const Location& b) {
    Location result = {a.x + b.x, a.y + b.y};
    return result;
}

inline Location operator-(const Location& a, const Location& b) {
    Location result = {a.x - b.x, a.y - b.y};
    return result;
}

inline Location operator-(const Location& a) {
    Location result = {-a.x, -a.y};
    return result;
}

inline Location operator*(const int a, const Location& b) {
    Location result = {a * b.x, a * b.y};
    return result;
}

inline Location operator*(const Location& a, const int b) {
    Location result = {b * a.x, b * a.y};
    return result;
}

extern const Location NoneLoc;

class Grid
{
private:
    int width;
    int height;
    unsigned char* walls_mask;
    int capacity;

    void ensure_capacity(int size);

    // Disable copying
    Grid(const Grid&);
    Grid& operator=(const Grid&);

public:
    Grid();
    ~Grid();

    void reset(int width_, int height_);
    void set_blocked(const Location& loc, bool blocked);

    int get_width() const { return width; }
    int get_height() const { return height; }

    inline int to_index(const Location& loc) const { return loc.y * width + loc.x; }
    inline Location from_index(int idx) const { return make_location(idx % width, idx / width); }
    inline int grid_size() const { return width * height; }

    bool in_bounds(const Location& loc) const { return 0 <= loc.x && loc.x < width && 0 <= loc.y && loc.y < height; }
    bool passable(const Location& loc) const;
    bool valid_move(const Location& loc, const Location& dir) const;
    bool forced(const Location& loc, const Location& parent, const Location& travel_dir) const;

    int neighbours(const Location& current, const Location* dirs, int dir_count, Location* out, int max_count) const;
    int pruned_neighbours(const Location& current, const Location& parent, Location* out, int max_count) const;
};
