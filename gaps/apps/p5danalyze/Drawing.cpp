#include "R3Graphics/R3Graphics.h"
#include "R3Graphics/p5d.h"
#include "GraphAux.h"
#include "Drawing.h"
#include <vector>

/*-------------------------------------------------------------------------*/
//  Drawing Methods
/*-------------------------------------------------------------------------*/


XformValues CreateXformValues(R3SceneNode* node, int resolution, bool is_room) {
    // Translation constants
    float a = 0.5 * (resolution - 1) - node->Centroid().X();
    float b = 0.5 * (resolution - 1) - node->Centroid().Y();
    float c = 0.5 * (resolution - 1) - node->Centroid().Z();

    R2Vector translation(a, b);
    R3Vector translation3D(a, b, c);

    if (is_room) {
        XformValues values = { translation, translation3D, 0.0, false, false, NULL };
        return values;
    }

    // Rotation constants
    P5DObject *p5d_obj = (P5DObject *) node->Data();
    RNAngle theta = p5d_obj->a;

    if (!strcmp(p5d_obj->className, "Door")) theta += RN_PI_OVER_TWO;
    else if (!strcmp(p5d_obj->className, "Window")) theta += RN_PI_OVER_TWO;

    bool do_fX = p5d_obj->fX;
    bool do_fY = p5d_obj->fY;

    XformValues values = { translation, translation3D, theta, do_fX, do_fY, NULL };
    return values;
}

// Prepares specific transform required by Heatmaps
R2Affine PrepareWorldToGridXform(R3Point cen3, XformValues values, int pixels_to_meters) {
    R2Vector cen = R2Vector(cen3.X(), cen3.Y());

    // Start with the identity matrix
    R2Affine world_to_grid_xform = R2identity_affine;

    // Transform the distance form src_obj
    if (values.dist != NULL) {
        values.dist->Rotate(-1.0 * values.theta);
        if (values.do_fX) values.dist->Mirror(R2posy_line);
        if (values.do_fY) values.dist->Mirror(R2posx_line);
        world_to_grid_xform.Translate(*(values.dist) * pixels_to_meters);
        world_to_grid_xform.Translate(-1.0 * *(values.dist));
    }

    // Center
    world_to_grid_xform.Translate(values.translation);

    // Transform about the origin
    world_to_grid_xform.Translate(cen);
    if (values.do_fX) world_to_grid_xform.XMirror();
    if (values.do_fY) world_to_grid_xform.YMirror();
    world_to_grid_xform.Scale(pixels_to_meters);
    world_to_grid_xform.Rotate(-1.0 * values.theta);
    world_to_grid_xform.Translate(-1.0 * cen);

    return world_to_grid_xform;
}

R3Affine PrepareWorldToGridXform(R3Point cen_pnt, XformValues values) 
{
    R3Vector cen = R3Vector(cen_pnt.X(), cen_pnt.Y(), cen_pnt.Z());

    // Start with the identity matrix
    R3Affine world_to_grid_xform = R3identity_affine;

    // Transform the dist3Dance from src_node
    if (values.dist3D != NULL) {
        values.dist3D->Rotate(R3posz_vector, -1.0 * values.theta);
        if (values.do_fX) values.dist3D->Mirror(R3posyz_plane);
        if (values.do_fY) values.dist3D->Mirror(R3posxz_plane);
        world_to_grid_xform.Translate(*(values.dist3D));
        world_to_grid_xform.Translate(-1.0 * *(values.dist3D));
    }

    // Center
    world_to_grid_xform.Translate(values.translation3D);

    // Transform about the origin
    world_to_grid_xform.Translate(cen);
    if (values.do_fX) world_to_grid_xform.XMirror();
    if (values.do_fY) world_to_grid_xform.YMirror();
    world_to_grid_xform.Rotate(R3posz_vector, -1.0 * values.theta);
    world_to_grid_xform.Translate(-1.0 * cen);

    return world_to_grid_xform;
}

R2Grid* DrawElement(R3SceneElement* el, R2Grid *grid, XformValues values, int pixels_to_meters, R3Point cen) {
    R2Grid temp_grid = R2Grid(grid->XResolution(), grid->YResolution());

    R2Affine world_to_grid_xform = PrepareWorldToGridXform(cen, values, pixels_to_meters);
    temp_grid.SetWorldToGridTransformation(world_to_grid_xform);

    // For all R3Shapes in the R3SceneElements    
    for (int l = 0; l < el->NShapes(); l++) {

        R3Shape* shape = el->Shape(l);
        R3TriangleArray* arr = (R3TriangleArray*) shape;

        // For all R3Triangles in the R3TriangleArray
        for (int t = 0; t < arr->NTriangles(); t++) {
            R3Triangle *triangle = arr->Triangle(t);

            // Create new points
            R2Point v0 = R2Point(triangle->V0()->Position().X(), triangle->V0()->Position().Y());
            R2Point v1 = R2Point(triangle->V1()->Position().X(), triangle->V1()->Position().Y());
            R2Point v2 = R2Point(triangle->V2()->Position().X(), triangle->V2()->Position().Y());

            if (IsOutsideGrid(&temp_grid, v0, v1, v2)) continue;

            // Move exterior verticies inside the grid
            std::vector<R2Point> v = MoveInsideGrid(&temp_grid, v0, v1, v2);

            v0 = temp_grid.GridPosition(v[0]);
            v1 = temp_grid.GridPosition(v[1]);
            v2 = temp_grid.GridPosition(v[2]);

            temp_grid.RasterizeWorldTriangle(v[0], v[1], v[2], 1);
        }
    }

    // Color the shape with a single color
    temp_grid.Threshold(0, 0, 1);
    grid->Add(temp_grid);
    return grid;
}



/* For Objects... */

R2Grid* DrawObject(R3SceneNode* obj, R2Grid *grid, XformValues values, int pixels_to_meters)
{
    R2Grid temp_grid = R2Grid(grid->XResolution(), grid->YResolution());

    R2Affine world_to_grid_xform = PrepareWorldToGridXform(obj->Centroid(), values, pixels_to_meters);
    temp_grid.SetWorldToGridTransformation(world_to_grid_xform);

    // For all R3SceneElements in the R3SceneNode
    for (int k = 0; k < obj->NElements(); k++) {
        R3SceneElement* el = obj->Element(k);

        // For all R3Shapes in the R3SceneElements    
        for (int l = 0; l < el->NShapes(); l++) {

            R3Shape* shape = el->Shape(l);
            R3TriangleArray* arr = (R3TriangleArray*) shape;

            // For all R3Triangles in the R3TriangleArray
            for (int t = 0; t < arr->NTriangles(); t++) {
                R3Triangle *triangle = arr->Triangle(t);

                // Create new points
                R2Point v0 = R2Point(triangle->V0()->Position().X(), triangle->V0()->Position().Y());
                R2Point v1 = R2Point(triangle->V1()->Position().X(), triangle->V1()->Position().Y());
                R2Point v2 = R2Point(triangle->V2()->Position().X(), triangle->V2()->Position().Y());

                if (IsOutsideGrid(&temp_grid, v0, v1, v2)) continue;

                // Move exterior verticies inside the grid
                std::vector<R2Point> v = MoveInsideGrid(&temp_grid, v0, v1, v2);

                v0 = temp_grid.GridPosition(v[0]);
                v1 = temp_grid.GridPosition(v[1]);
                v2 = temp_grid.GridPosition(v[2]);

                temp_grid.RasterizeWorldTriangle(v[0], v[1], v[2], 1);
            }
        }
    }

    // Color the shape with a single color
    temp_grid.Threshold(0, 0, 1);
    grid->Add(temp_grid);
    return grid;
}


R3Grid* DrawObject(R3SceneNode* node, R3Grid *grid, XformValues values)
{
    R3Grid temp_grid = R3Grid(grid->XResolution(), grid->YResolution(), grid->ZResolution());

    //XformValues values = CreateXformValues(node, grid->XResolution());
    R3Affine world_to_grid_xform = PrepareWorldToGridXform(node->Centroid(), values);

    // For all R3SceneElements in the R3SceneNode
    for (int k = 0; k < node->NElements(); k++) {
        R3SceneElement* el = node->Element(k);

        // For all R3Shapes in the R3SceneElements    
        for (int l = 0; l < el->NShapes(); l++) {

            R3Shape* shape = el->Shape(l);
            R3TriangleArray* arr = (R3TriangleArray*) shape;

            // For all R3Triangles in the R3TriangleArray
            for (int t = 0; t < arr->NTriangles(); t++) {
                R3Triangle *triangle = arr->Triangle(t);

                // Create new points
                R3Point v0 = R3Point(triangle->V0()->Position());
                R3Point v1 = R3Point(triangle->V1()->Position());
                R3Point v2 = R3Point(triangle->V2()->Position());

                if (IsOutsideGrid(&temp_grid, v0, v1, v2)) continue;

                // Move exterior verticies inside the grid
                std::vector<R3Point> v = MoveInsideGrid(&temp_grid, v0, v1, v2);
                temp_grid.RasterizeWorldTriangle(v[0], v[1], v[2], 1);
            }
        }
    }

    // Color the shape with a single color
    temp_grid.Threshold(0, 0, 1);
    grid->Add(temp_grid);
    return grid;
}

