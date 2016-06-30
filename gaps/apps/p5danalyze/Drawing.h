// This is the header file for Drawing

#ifndef DRAWING
#define DRAWING

#include "R3Graphics/R3Graphics.h"

struct XformValues {
    R2Vector translation;
    R3Vector translation3D;
    RNAngle theta;
    bool do_fX;
    bool do_fY;
    R2Vector *dist;
    R3Vector *dist3D;
};

XformValues CreateXformValues(R3SceneNode* node, int resolution = 0);

R2Affine PrepareWorldToGridXform(R3Point cen3, XformValues values, int pixels_to_meters);
R3Affine PrepareWorldToGridXform(R3Point cen3, XformValues values);


// Prepares specific transform required by Heatmaps
R2Grid* DrawObject(R3SceneNode* obj, R2Grid *grid, XformValues values, int pixels_to_meters = 1);
R3Grid* DrawObject(R3SceneNode* node, R3Grid *grid, XformValues values);

#endif 
