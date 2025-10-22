#pragma once

#include "grid.hpp"

typedef double(heuristic_fn)(const Location&, const Location&);

Location jump(const Grid& grid, const Location initial, const Location dir,
    const Location goal);

int successors(const Grid& grid, const Location& current,
    const Location& parent, const Location& goal,
    Location* out, int max_count);

int jps_find_path(
    const Grid& grid,
    const Location& start, const Location& goal,
    heuristic_fn heuristic,
    Location* out_path,
    int max_path_length);

void jps_shutdown();
