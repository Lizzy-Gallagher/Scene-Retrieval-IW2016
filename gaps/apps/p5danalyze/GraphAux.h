// This is the header file for "Grid Actions"

#ifndef GRID_AUX
#define GRID_AUX

#include "R3Graphics/R3Graphics.h"
#include <vector>

using TriangleR2 = std::vector<R2Point>;
using TriangleR3 = std::vector<R3Point>;

// Moves vertex inside grid
TriangleR2 MoveInsideGrid(R2Grid* grid, R2Point v0, R2Point v1, R2Point v2);
TriangleR3 MoveInsideGrid(R3Grid* grid, R3Point v0, R3Point v1, R3Point v2);

// Verifies whether vertices are outside of the grid
bool IsOutsideGrid(R2Grid* grid, R2Point v0, R2Point v1, R2Point v2);
bool IsOutsideGrid(R3Grid* grid, R3Point v0, R3Point v1, R3Point v2);



#endif
