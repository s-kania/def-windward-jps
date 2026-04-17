# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.1.1] - 2026-04-17

### Fixed

- Build failures on Windows with Defold SDK 1.11.2+ caused by the Windows system headers (`minwindef.h`, `malloc.h`) requiring `constexpr` and `noexcept`, which are unavailable under `-std=c++98`. The extension now compiles with `-std=c++11`. Existing extension source code is unchanged and remains compatible. ([#2](https://github.com/s-kania/def-windward-jps/issues/2))

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
