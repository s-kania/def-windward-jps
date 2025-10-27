# def_windward_jps – Jump Point Search extension for Defold

Native Defold extension implementing the Jump Point Search (JPS) algorithm for fast pathfinding on 2D grids. The code is based on the [hvillanua/jps](https://github.com/hvillanua/jps/) implementation and exposes a straightforward Lua API for configuring obstacles and requesting paths.

## Features

- stores grid state in native memory,
- supports 8-direction movement (with the octile heuristic),
- allows choosing heuristics (`octile`, `manhattan`, `euclidean`),
- returns a path as an array of points ready for Lua usage.

## Installation

### 1. Add as dependency (recommended)

In your `game.project`, add the repository as a dependency – you can point to a branch, tag, or release zip:

```
[project]
dependencies = https://github.com/kanias/def-jps/archive/refs/heads/main.zip
```

If you use your own fork or specific version, replace the URL with the proper Defold-compatible link.

### 2. Manual installation

1. Clone the repository or download the `.zip` archive.
2. Copy the `def_windward_jps` directory into your Defold project.
3. Ensure the folder resides within Defold’s script search paths (for example, in the project root).

## Project setup

The extension registers a Lua module named `def_windward_jps`. You can explicitly require it:

```lua
local jps = require("def_windward_jps.def_windward_jps")
```

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

## Example usage in a script

The following snippet demonstrates the complete flow: converting a Defold grid to the required structure, initializing walls, and requesting a path. It is based on the demo scene available in the `main/` directory.

```lua
local grid_generator = require("main.grid_generator")
local jps = require("def_windward_jps.def_windward_jps")

local function grid_to_walls(grid, grid_size)
    local walls = {}
    for y = 1, grid_size do
        for x = 1, grid_size do
            if grid[y][x] == 1 then
                table.insert(walls, {x, y})
            end
        end
    end
    return walls
end

function init(self)
    self.grid, self.grid_size = grid_generator.create_island_grid()
    self.walls = grid_to_walls(self.grid, self.grid_size)

    self.jps_grid = jps.create_grid(self.grid_size, self.grid_size, self.walls)

    local start = {22, 146}
    local goal = {270, 146}

    local path, err = self.jps_grid:find_path(start, goal, "octile")
    if path then
        for _, point in ipairs(path) do
            -- process the result (e.g., visualization)
        end
    else
        pprint(err)
    end
end
```

## Troubleshooting

1. **"grid not initialized"** – ensure `create_grid` is called before `find_path`.
2. **Start/goal blocked** – verify that start/goal points are not part of the `walls` list and remain within bounds.
3. **No path found** – the algorithm returns an error if there is no connection between points given the obstacle layout.

## Examples

The repository ships with a demo scene (`main/`) that generates a random island, sets up the grid, and visualizes the computed path. You can run the project without extra configuration to see the extension in action.

## License

The project is released under the MIT license (see `LICENSE.md`).
