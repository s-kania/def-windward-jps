-- Simplex Noise Implementation for Lua
-- Adapted from https://github.com/jwagner/simplex-noise.js

local M = {}

local F2 = 0.5 * (math.sqrt(3.0) - 1.0)
local G2 = (3.0 - math.sqrt(3.0)) / 6.0

local SimplexNoise = {}
SimplexNoise.__index = SimplexNoise

function SimplexNoise.new(seed)
    local self = setmetatable({}, SimplexNoise)
    
    -- Initialize permutation table
    math.randomseed(seed or os.time())
    self.p = {}
    for i = 0, 255 do
        self.p[i] = math.random(0, 255)
    end
    
    self.perm = {}
    self.permMod12 = {}
    for i = 0, 511 do
        self.perm[i] = self.p[i % 256]
        self.permMod12[i] = self.perm[i] % 12
    end
    
    -- Gradient vectors for 2D
    self.grad3 = {
        1, 1, 0,  -1, 1, 0,  1, -1, 0,  -1, -1, 0,
        1, 0, 1,  -1, 0, 1,  1, 0, -1,  -1, 0, -1,
        0, 1, 1,  0, -1, 1,  0, 1, -1,  0, -1, -1
    }
    
    return self
end

function SimplexNoise:noise2D(xin, yin)
    local n0, n1, n2 = 0, 0, 0
    
    -- Skew the input space
    local s = (xin + yin) * F2
    local i = math.floor(xin + s)
    local j = math.floor(yin + s)
    
    local t = (i + j) * G2
    local X0 = i - t
    local Y0 = j - t
    local x0 = xin - X0
    local y0 = yin - Y0
    
    -- Determine which simplex we are in
    local i1, j1
    if x0 > y0 then
        i1, j1 = 1, 0
    else
        i1, j1 = 0, 1
    end
    
    -- Offsets for second and third corners
    local x1 = x0 - i1 + G2
    local y1 = y0 - j1 + G2
    local x2 = x0 - 1.0 + 2.0 * G2
    local y2 = y0 - 1.0 + 2.0 * G2
    
    -- Work out the hashed gradient indices
    local ii = i % 256
    local jj = j % 256
    
    -- Calculate the contribution from the three corners
    local t0 = 0.5 - x0 * x0 - y0 * y0
    if t0 >= 0 then
        t0 = t0 * t0
        local gi0 = self.permMod12[ii + self.perm[jj]]
        n0 = t0 * t0 * (self.grad3[gi0 * 3 + 1] * x0 + self.grad3[gi0 * 3 + 2] * y0)
    end
    
    local t1 = 0.5 - x1 * x1 - y1 * y1
    if t1 >= 0 then
        t1 = t1 * t1
        local gi1 = self.permMod12[ii + i1 + self.perm[jj + j1]]
        n1 = t1 * t1 * (self.grad3[gi1 * 3 + 1] * x1 + self.grad3[gi1 * 3 + 2] * y1)
    end
    
    local t2 = 0.5 - x2 * x2 - y2 * y2
    if t2 >= 0 then
        t2 = t2 * t2
        local gi2 = self.permMod12[ii + 1 + self.perm[jj + 1]]
        n2 = t2 * t2 * (self.grad3[gi2 * 3 + 1] * x2 + self.grad3[gi2 * 3 + 2] * y2)
    end
    
    -- Add contributions from each corner and scale to [-1, 1]
    return 70.0 * (n0 + n1 + n2)
end

M.SimplexNoise = SimplexNoise

return M
