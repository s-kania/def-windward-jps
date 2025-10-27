<img width="350" height="350" alt="def-windward-jps-logo" src="https://github.com/user-attachments/assets/5aa1830f-964c-4e9d-8ce7-2aad38aeda45" />


Native Defold extension implementing the Jump Point Search (JPS) algorithm for fast pathfinding on 2D grids. The code is based on the [hvillanua/jps](https://github.com/hvillanua/jps/) implementation and exposes a straightforward Lua API for configuring obstacles and requesting paths.

## Features

- stores grid state in native memory,
- supports 8-direction movement (with the octile heuristic),
- allows choosing heuristics (`octile`, `manhattan`, `euclidean`),
- returns a path as an array of points ready for Lua usage,
- native code compiled with the C++98 standard for full Defold compatibility.

## Installation

### Add as dependency

In your `game.project`, add the repository as a dependency – you can point to a branch, tag, or release zip:

```
[project]
dependencies = https://github.com/kanias/def-jps/archive/refs/heads/main.zip
```

If you use your own fork or specific version, replace the URL with the proper Defold-compatible link.

## API

### `def_windward_jps.create_grid(width, height, walls)`

- `width`, `height` – grid dimensions (integers > 0).
- `walls` – array of blocked points in the form `{ {x1, y1}, {x2, y2}, ... }`.

Creates and returns a new grid instance for pathfinding. Multiple grids can be used simultaneously.

### `grid:find_path(start, goal, heuristic?)`

- `start`, `goal` – tables `{x, y}`.
- `heuristic` (optional) – heuristic name (`"octile"`, `"manhattan"`, `"euclidean"`). Defaults to `"octile"`.

Returns two values: the path as an array `{ {x1, y1}, ... }` and `nil` as the error message. On failure, returns `nil` plus an error description (e.g., grid not initialized, blocked start/goal, no path).

This method operates on a specific grid instance returned by `create_grid`.

## Quick example

Once the extension is added as a dependency, Defold exposes it under the global `def_windward_jps` namespace – no `require` call needed. A minimal usage example:

```lua
local walls = {
    {10, 12},
    {11, 12},
    {12, 12},
}

local grid = def_windward_jps.create_grid(64, 64, walls)

local start = {8, 8}
local goal = {40, 40}

local path, err = grid:find_path(start, goal, "octile")
if not path then
    pprint(err)
end
```

For a full end-to-end example (grid generation, rendering, GUI), see the demo scene in the `main/` directory.

## Troubleshooting

1. **"grid not initialized"** – ensure `create_grid` is called before `find_path`.
2. **Start/goal blocked** – verify that start/goal points are not part of the `walls` list and remain within bounds.
3. **No path found** – the algorithm returns an error if there is no connection between points given the obstacle layout.

## Examples

The repository ships with a demo scene (`main/`) that generates a random island, sets up the grid, and visualizes the computed path. You can run the project without extra configuration to see the extension in action.

For the default grid size of 288x288, pathfinding typically takes approximately 1ms on modern hardware.
<img width="958" height="603" alt="Screenshot 2025-10-27 at 12 00 28" src="https://github.com/user-attachments/assets/1044283f-227b-4711-8e92-06536a603a48" />


## License

The project is released under the MIT license (see `LICENSE.md`).
