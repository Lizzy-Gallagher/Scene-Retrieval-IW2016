#include "GraphAux.h"

/*-------------------------------------------------------------------------*/
//  Graph Methods
/*-------------------------------------------------------------------------*/

// Moves vertex inside grid
R2Point MoveInsideGrid(R2Grid* grid, R2Point v) {
    R2Point grid_v = grid->GridPosition(v);
    RNScalar x = grid_v.X();
    RNScalar y = grid_v.Y();

    if (x >= grid->XResolution())
        x = grid->XResolution() - 1;
    else if (x < 0)
        x = 0;
   
    if (y >= grid->YResolution())
        y = grid->YResolution() - 1;
    else if (y < 0)
        y = 0;

    return grid->WorldPosition(int(x), int(y));
}

TriangleR2 MoveInsideGrid(R2Grid* grid, R2Point v0, R2Point v1, R2Point v2) {
    v0 = MoveInsideGrid(grid, v0);
    v1 = MoveInsideGrid(grid, v1);
    v2 = MoveInsideGrid(grid, v2);

    TriangleR2 v = { v0, v1, v2 };
    return v;
}

// Verifies whether vertices are outside of the grid
bool IsOutsideGrid(R2Grid* grid, R2Point v) {
    v = grid->GridPosition(v);

    return v.X() >= grid->XResolution() || v.X() < 0 ||
        v.Y() >= grid->YResolution() || v.Y() < 0;
}

bool IsOutsideGrid(R2Grid* grid, R2Point v0, R2Point v1, R2Point v2) {
    return IsOutsideGrid(grid, v0) && IsOutsideGrid(grid, v1) && IsOutsideGrid(grid, v2);
}

// Moves vertex inside grid
R3Point MoveInsideGrid(R3Grid* grid, R3Point v) {
    R3Point grid_v = grid->GridPosition(v);
    RNScalar x = grid_v.X();
    RNScalar y = grid_v.Y();
    RNScalar z = grid_v.Z();

    if (x >= grid->XResolution())
        x = grid->XResolution() - 1;
    else if (x < 0)
        x = 0;
   
    if (y >= grid->YResolution())
        y = grid->YResolution() - 1;
    else if (y < 0)
        y = 0;

    if (z >= grid->ZResolution())
        z = grid->ZResolution() - 1;
    else if (z < 0)
        z = 0;

    return grid->WorldPosition(int(x), int(y), int(z));
}

TriangleR3 MoveInsideGrid(R3Grid* grid, R3Point v0, R3Point v1, R3Point v2) {
    v0 = MoveInsideGrid(grid, v0);
    v1 = MoveInsideGrid(grid, v1);
    v2 = MoveInsideGrid(grid, v2);

    TriangleR3 v = { v0, v1, v2 };
    return v;
}

// Verifies whether vertices are outside of the grid
bool IsOutsideGrid(R3Grid* grid, R3Point v) {
    v = grid->GridPosition(v);

    return v.X() >= grid->XResolution() || v.X() < 0 ||
        v.Y() >= grid->YResolution() || v.Y() < 0 ||
        v.Z() >= grid->ZResolution() || v.Z() < 0;
}

bool IsOutsideGrid(R3Grid* grid, R3Point v0, R3Point v1, R3Point v2) {
    return IsOutsideGrid(grid, v0) && IsOutsideGrid(grid, v1) && IsOutsideGrid(grid, v2);
}
