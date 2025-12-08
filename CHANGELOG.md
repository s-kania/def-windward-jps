# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.1.0] - 2025-12-08

### Added

- `grid:is_walkable(point)` – new API method to check if a specific point is walkable (within bounds and not blocked).

## [1.0.0] - 2025-10-27

### Added

- Initial release of def-windward-jps extension.
- `def_windward_jps.create_grid(width, height, walls)` – create a grid instance for pathfinding.
- `grid:find_path(start, goal, heuristic?)` – find path using Jump Point Search algorithm.
- Support for three heuristics: `octile`, `manhattan`, `euclidean`.
- 8-direction movement support.
- Native C++98 implementation for full Defold compatibility.
