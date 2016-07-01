#include "R3Graphics/R3Graphics.h"
#include "P5DAux.h"
#include "Drawing.h"

#include <string>
#include <vector>

SceneNodes GetSceneNodes(R3Scene *scene)
{
    SceneNodes nodes;

    // Find all objects and walls
    for (int i = 0; i < scene->NNodes(); i++) {
        R3SceneNode* node = scene->Node(i);

        std::string name (node->Name());
        if (std::string::npos != name.find("Object")) // probably a better way with casting
            nodes.objects.push_back(node);
        if (std::string::npos != name.find("Wall"))
            nodes.walls.push_back(node);     
    }

    return nodes;
}

struct Wall {
    bool isHorizon;
    bool isVert;

    float x;
    float y;

    std::vector<R3SceneElement*> elements;
};

std::vector<Wall> walls;

// If within some small threshold

bool IsHorizontalWall(R3Triangle* triangle) {
    R3Point v0 = triangle->V0()->Position();
    R3Point v1 = triangle->V1()->Position();
    R3Point v2 = triangle->V2()->Position();
    
    double ave = (v0.Y() + v1.Y() + v2.Y()) / 3.0;
    return (fabs(ave - v0.Y()) > 0.1);
}

bool IsVerticalWall(R3Triangle* triangle) {
    R3Point v0 = triangle->V0()->Position();
    R3Point v1 = triangle->V1()->Position();
    R3Point v2 = triangle->V2()->Position();
    
    double ave = (v0.X() + v1.X() + v2.X()) / 3.0;
    return (fabs(ave - v0.X()) > 0.1);
}

float GetX(R3Triangle* triangle) {
    R3Point v0 = triangle->V0()->Position();
    R3Point v1 = triangle->V1()->Position();
    R3Point v2 = triangle->V2()->Position();

    return (v0.X() + v1.X() + v2.X()) / 3.0;
}

float GetY(R3Triangle* triangle) {
    R3Point v0 = triangle->V0()->Position();
    R3Point v1 = triangle->V1()->Position();
    R3Point v2 = triangle->V2()->Position();

    return (v0.Y() + v1.Y() + v2.Y()) / 3.0;
}


Walls GetWalls(R3SceneNode* room) {
    int resolution = 500;
    R2Grid* grid = new R2Grid(resolution, resolution);
    XformValues values = CreateXformValues(room, resolution, true);

    //fprintf(stdout, "Number of Elements: %d\n", room->NElements());
    for (int k = 0; k < room->NElements(); k++) {
       R3SceneElement* el = room->Element(k); 
       //fprintf(stdout, "\tNumber of Shapes in el %d: %d\n", k, el->NShapes());
       
       for (int l = 0; l < el->NShapes(); l++) {
            R3Shape* shape = el->Shape(l);
            R3TriangleArray* arr = (R3TriangleArray*) shape;

            // For all R3Triangles in the R3TriangleArray
            for (int t = 0; t < arr->NTriangles(); t++) {
                R3Triangle *triangle = arr->Triangle(t);
               
                Wall wall = {};
                wall.elements.push_back(el);

                if (IsHorizontalWall(triangle)) {
                    wall.isHorizon = true;
                    wall.y = GetY(triangle);
                } else {
                    wall.isVert = true;
                    wall.x = GetX(triangle);
                }

                // Check if matches a previous wall
                if (walls.size() == 0) {
                    walls.push_back(wall); // add first wall
                    continue;
                }
                
                bool already_wall = false;
                for (Wall est_wall : walls) {
                    if (est_wall.isHorizon != wall.isHorizon) continue;
                    if (fabs(est_wall.x - wall.x) > 0.1 &&
                        fabs(est_wall.y - wall.y) > 0.1) continue;

                    fprintf(stdout, "Same wall!\n");
                    est_wall.elements.push_back(el);
                    already_wall = true;
                    //break;
                }

                if (!already_wall)
                    walls.push_back(wall);
                
                fprintf(stdout, "size %lu\n", walls.size()); 
                // Perhaps collapse the notion of "wall" from a mesh into a BB (possibly first step)
            }
       }
    } 

    fprintf(stdout, "Num Walls: %lu\n", walls.size());

    R2Grid* drawn_grid = DrawObject(room, grid, values, 70); 
    drawn_grid->WriteImage("room.png");
    exit(1);
    return Walls();
}

