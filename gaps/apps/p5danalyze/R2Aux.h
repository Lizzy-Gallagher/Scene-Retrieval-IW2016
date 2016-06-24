// This is the header file for the 2-D manipulations used in heatmap
// generation. Moved for the sake of clarity.

#ifndef R2_AUX
#define R2_AUX

#include "R3Graphics/R3Graphics.h"
#include <vector>

struct DrawingValues {
    R2Vector translation;
    RNAngle theta;
    bool do_fX;
    bool do_fY;
    R2Vector *dist;
};

// Moves vertex inside grid
std::vector<R2Point> MoveInsideGrid(R2Grid* grid, R2Point v0, R2Point v1, R2Point v2);

// Verifies whether vertices are outside of the grid
bool IsOutsideGrid(R2Grid* grid, R2Point v0, R2Point v1, R2Point v2);

// Prepares specific transform required by Heatmaps
R2Grid* DrawObject(R3SceneNode* obj, R2Grid *grid, DrawingValues values, int pixels_to_meters);

#endif 
