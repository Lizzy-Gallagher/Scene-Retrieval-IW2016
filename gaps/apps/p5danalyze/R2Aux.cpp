#include "R3Graphics/R3Graphics.h"
#include <vector>

// Moves vertex inside grid
R2Point 
MoveInsideGrid(R2Grid* grid, R2Point v) {
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

std::vector<R2Point> 
MoveInsideGrid(R2Grid* grid, R2Point v0, R2Point v1, R2Point v2) {
    v0 = MoveInsideGrid(grid, v0);
    v1 = MoveInsideGrid(grid, v1);
    v2 = MoveInsideGrid(grid, v2);

    std::vector<R2Point> v = { v0, v1, v2 };
    return v;
}

// Verifies whether vertices are outside of the grid
bool 
IsOutsideGrid(R2Grid* grid, R2Point v) {
    v = grid->GridPosition(v);

    return v.X() >= grid->XResolution() || v.X() < 0 ||
        v.Y() >= grid->YResolution() || v.Y() < 0;
}

bool IsOutsideGrid(R2Grid* grid, R2Point v0, R2Point v1, R2Point v2) {
    return IsOutsideGrid(grid, v0) && IsOutsideGrid(grid, v1) && IsOutsideGrid(grid, v2);
}

// Prepares specific transform required by Heatmaps
R2Affine 
PrepareWorldToGridXform(R3Point cen3, R2Vector translation, RNAngle theta, bool do_fX, bool do_fY, R2Vector* dist, int pixels_to_meters) {
    R2Vector cen = R2Vector(cen3.X(), cen3.Y());
    
    // Start with the identity matrix
    R2Affine world_to_grid_xform = R2identity_affine;
    
    // Transform the distance from src_obj
    if (dist != NULL) {
        dist->Rotate(-1.0 * theta);
        if (do_fX) dist->Mirror(R2posy_line);
        if (do_fY) dist->Mirror(R2posx_line);
        world_to_grid_xform.Translate(*dist * pixels_to_meters);
        world_to_grid_xform.Translate(-1.0 * *dist);
    }
    
    // Center
    world_to_grid_xform.Translate(translation);

    // Transform about the origin
    world_to_grid_xform.Translate(cen);
    if (do_fX) world_to_grid_xform.XMirror();
    if (do_fY) world_to_grid_xform.YMirror();
    world_to_grid_xform.Scale(pixels_to_meters);
    world_to_grid_xform.Rotate(-1.0 * theta);
    world_to_grid_xform.Translate(-1.0 * cen);

    return world_to_grid_xform;
}


