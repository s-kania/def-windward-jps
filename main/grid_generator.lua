-- Grid Generator with Island Generation
-- Adapted from the React pathfinding visualizer

local simplex_noise = require("main.simplex_noise")

local M = {}

local GRID_SIZE = 288
local DEFAULT_SEED = 42  -- Fixed seed for consistent grid generation

function M.create_island_grid(seed)
    seed = seed or DEFAULT_SEED
    
    -- Initialize grid (0 = walkable, 1 = obstacle/land)
    local grid = {}
    for y = 1, GRID_SIZE do
        grid[y] = {}
        for x = 1, GRID_SIZE do
            grid[y][x] = 0
        end
    end
    
    local noise = simplex_noise.SimplexNoise.new(seed)
    
    local centerX = GRID_SIZE / 2
    local centerY = GRID_SIZE / 2
    local scale = 0.045  -- Controls the "zoom" of the noise
    local islandThreshold = 0.18  -- Threshold to determine land vs water
    
    for y = 1, GRID_SIZE do
        for x = 1, GRID_SIZE do
            -- Calculate distance from center (convert to 0-indexed for consistency)
            local dx = (x - 1) - centerX
            local dy = (y - 1) - centerY
            local dist = math.sqrt(dx * dx + dy * dy)
            
            -- Create a radial gradient (0 at center, 1 at edge)
            local gradient = dist / (GRID_SIZE / 2)
            
            -- Get noise value, mapped from [-1, 1] to [0, 1]
            local noiseVal = (noise:noise2D((x - 1) * scale, (y - 1) * scale) + 1) / 2
            
            -- Subtract the gradient from the noise
            local finalVal = noiseVal - math.pow(gradient, 1.2)
            
            if finalVal > islandThreshold then
                grid[y][x] = 1  -- 1 represents an obstacle (land)
            end
        end
    end
    
    return grid, GRID_SIZE
end

return M
