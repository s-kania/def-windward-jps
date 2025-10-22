#include "grid.hpp"
#include <limits.h>

static const Location ALL_DIRS[8] = {
    {1, 0}, {-1, 0},
    {0, -1}, {0, 1},
    {1, 1}, {-1, 1},
    {1, -1}, {-1, -1}
};

const Location NoneLoc = {-1, -1};

bool operator<(const Location& a, const Location& b)
{
    if(a.x < b.x) return true;
    if(a.x > b.x) return false;
    return a.y < b.y;
}

Grid::Grid()
    : width(0)
    , height(0)
    , walls_mask(0)
    , capacity(0)
{
}

Grid::~Grid()
{
    if(walls_mask != 0) {
        delete[] walls_mask;
        walls_mask = 0;
    }
    capacity = 0;
}

void Grid::ensure_capacity(int size)
{
    if(size <= capacity) {
        return;
    }

    unsigned char* new_mask = new unsigned char[size];
    int i;
    for(i = 0; i < size; ++i) {
        new_mask[i] = 0;
    }

    if(walls_mask != 0) {
        for(i = 0; i < capacity; ++i) {
            new_mask[i] = walls_mask[i];
        }
        delete[] walls_mask;
    }

    walls_mask = new_mask;
    capacity = size;
}

void Grid::reset(int width_, int height_)
{
    width = width_;
    height = height_;

    if(width > 0 && height > 0 && width > (INT_MAX / height)) {
        width = 0;
        height = 0;
        return;
    }

    int size = width * height;
    ensure_capacity(size);

    int i;
    for(i = 0; i < size; ++i) {
        walls_mask[i] = 0;
    }
}

void Grid::set_blocked(const Location& loc, bool blocked)
{
    if(!in_bounds(loc)) {
        return;
    }
    walls_mask[to_index(loc)] = blocked ? 1 : 0;
}

bool Grid::passable(const Location& loc) const
{
    if(!in_bounds(loc)) {
        return false;
    }
    return walls_mask[to_index(loc)] == 0;
}

bool Grid::valid_move(const Location& loc, const Location& dir) const
{
    Location next_loc = loc + dir;
    if(dir.x != 0 && dir.y != 0) {
        Location dir_x = make_location(dir.x, 0);
        Location dir_y = make_location(0, dir.y);
        return in_bounds(next_loc) && passable(next_loc)
            && (passable(loc + dir_x) || passable(loc + dir_y));
    }
    return in_bounds(next_loc) && passable(next_loc);
}

bool Grid::forced(const Location& loc, const Location& parent, const Location& travel_dir) const
{
    Location dir = (loc - parent).direction();
    if(travel_dir.x != 0 && travel_dir.y != 0) {
        if((dir.x == travel_dir.x && dir.y == -travel_dir.y) ||
           (dir.x == -travel_dir.x && dir.y == travel_dir.y)) {
            return true;
        }
    }
    else if(dir.x != 0 && dir.y != 0) {
        return true;
    }
    return false;
}

int Grid::neighbours(const Location& current, const Location* dirs, int dir_count, Location* out, int max_count) const
{
    int count = 0;
    int i;
    for(i = 0; i < dir_count && count < max_count; ++i) {
        const Location& dir = dirs[i];
        if(valid_move(current, dir)) {
            out[count] = current + dir;
            ++count;
        }
    }
    return count;
}

int Grid::pruned_neighbours(const Location& current, const Location& parent, Location* out, int max_count) const
{
    if(parent == NoneLoc) {
        return neighbours(current, ALL_DIRS, 8, out, max_count);
    }

    int count = 0;
    Location dir = (current - parent).direction();

    if(dir.x != 0 && dir.y != 0) {
        Location dir_x = make_location(dir.x, 0);
        Location dir_y = make_location(0, dir.y);

        Location diagonal_dirs[3];
        diagonal_dirs[0] = dir;
        diagonal_dirs[1] = dir_x;
        diagonal_dirs[2] = dir_y;
        count = neighbours(current, diagonal_dirs, 3, out, max_count);

        Location previous = current - dir;
        Location forced_dirs[2];
        forced_dirs[0] = dir_x;
        forced_dirs[1] = dir_y;
        int i;
        for(i = 0; i < 2; ++i) {
            const Location& candidate_dir = forced_dirs[i];
            Location double_candidate = candidate_dir * 2;
            if(!valid_move(previous, candidate_dir) && valid_move(previous, double_candidate)) {
                if(count < max_count) {
                    out[count] = previous + double_candidate;
                    ++count;
                }
            }
        }
    }
    else {
        Location cardinal_dir[1];
        cardinal_dir[0] = dir;
        count = neighbours(current, cardinal_dir, 1, out, max_count);

        Location inverted_dir = make_location(dir.y, dir.x);
        Location neg_inverted_dir = make_location(-inverted_dir.x, -inverted_dir.y);
        Location inverted_plus_dir = inverted_dir + dir;
        Location neg_inverted_plus_dir = neg_inverted_dir + dir;
        if(!valid_move(current, inverted_dir) && valid_move(current, inverted_plus_dir)) {
            if(count < max_count) {
                out[count] = current + inverted_plus_dir;
                ++count;
            }
        }
        if(!valid_move(current, neg_inverted_dir) && valid_move(current, neg_inverted_plus_dir)) {
            if(count < max_count) {
                out[count] = current + neg_inverted_plus_dir;
                ++count;
            }
        }
    }

    return count;
}
