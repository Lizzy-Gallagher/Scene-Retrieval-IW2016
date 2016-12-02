#include "R3Graphics/R3Graphics.h"
#include "P5DAux.h"
#include "Drawing.h"

#include <string>
#include <vector>
#include <sstream>

SceneNodes GetSceneNodes(R3Scene *scene)
{
    SceneNodes nodes;

    // Find all objects and walls
    for (int i = 0; i < scene->NNodes(); i++) {
        R3SceneNode* node = scene->Node(i);

        std::string name (node->Name());
        if (std::string::npos != name.find("Object")) // probably a better way with casting
            nodes.objects.push_back(node);
        if (std::string::npos != name.find("Wall")) {
            std::vector<Wall*> walls = GetWalls(node);
            nodes.walls.reserve(nodes.walls.size() + walls.size());
            nodes.walls.insert(nodes.walls.end(), walls.begin(), walls.end());
        }
    }

    return nodes;
}


struct VerboseWall {
    // Stats for construction 
    bool isHorizon;
    bool isVert;

    float x = 0.0;
    float y = 0.0;

    R3TriangleArray* triangles;
};

// If within some small threshold

double GetAveX(R3Triangle* triangle) {
    R3Point v0 = triangle->V0()->Position();
    R3Point v1 = triangle->V1()->Position();
    R3Point v2 = triangle->V2()->Position();

    return (v0.X() + v1.X() + v2.X()) / 3.0;
}

double GetAveY(R3Triangle* triangle) {
    R3Point v0 = triangle->V0()->Position();
    R3Point v1 = triangle->V1()->Position();
    R3Point v2 = triangle->V2()->Position();

    return (v0.Y() + v1.Y() + v2.Y()) / 3.0;
}

double GetSampleX(R3Triangle* triangle) {
    R3Point v0 = triangle->V0()->Position();
    return v0.X();
}

double GetSampleY(R3Triangle* triangle) {
    R3Point v0 = triangle->V0()->Position();
    return v0.Y();
}

bool IsHorizontalWall(R3Triangle* triangle) {
    double ave = GetAveY(triangle);
    return (fabs(ave - GetSampleY(triangle)) < 0.1);
}

bool IsVerticalWall(R3Triangle* triangle) {
    double ave = GetAveX(triangle); 
    return (fabs(ave - GetAveX(triangle)) < 0.1);
}

R3TriangleArray* MergeTriangleArray(R3TriangleArray* arr1, R3TriangleArray* arr2) {
    // Make collection of vertices
    RNArray<R3TriangleVertex*> vertices = RNArray<R3TriangleVertex*>();
    for (int v = 0; v < arr1->NVertices(); v++)
        vertices.Insert(arr1->Vertex(v));
    for (int v = 0; v < arr2->NVertices(); v++)
        vertices.Insert(arr2->Vertex(v));
    
    // Make collection of triangles
    RNArray<R3Triangle*> triangles = RNArray<R3Triangle*>();
    for (int v = 0; v < arr1->NTriangles(); v++)
        triangles.Insert(arr1->Triangle(v));
    for (int v = 0; v < arr2->NTriangles(); v++)
        triangles.Insert(arr2->Triangle(v));

    return new R3TriangleArray(vertices, triangles); 
}

std::vector<Wall*> ConvertVerboseWalls(std::vector<VerboseWall>& v_walls) {
    std::vector<Wall*> walls;
    
    for (VerboseWall& v_wall : v_walls) {
        walls.push_back(v_wall.triangles);
    }

    return walls;
}

std::vector<Wall*> GetWalls(R3SceneNode* room) {
    std::vector<VerboseWall> walls;

    //int resolution = 500;
    //R2Grid* grid = new R2Grid(resolution, resolution);
    //XformValues values = CreateXformValues(room, resolution, true);

    for (int k = 0; k < room->NElements(); k++) {
        R3SceneElement* el = room->Element(k); 

        for (int l = 0; l < el->NShapes(); l++) {
            R3Shape* shape = el->Shape(l);
            R3TriangleArray* arr = (R3TriangleArray*) shape;

            VerboseWall wall = {};
            wall.triangles = arr;

            R3Triangle *triangle = arr->Triangle(0);
            if (IsHorizontalWall(triangle)) {
                wall.isHorizon = true;
                wall.y = GetAveY(triangle);
            } else {
                wall.isVert = true;
                wall.x = GetAveX(triangle);
            }

            // Check if matches a previous wall
            if (walls.size() == 0) {
                walls.push_back(wall); // add first wall
                continue;
            }

            bool already_wall = false;
            for (auto &est_wall : walls) {
                if (est_wall.isHorizon != wall.isHorizon) continue;

                if (est_wall.isHorizon && fabs(est_wall.y - wall.y) > 0.5) continue;
                if (est_wall.isVert && fabs(est_wall.x - wall.x) > 0.5) continue;

                //est_wall.triangles->push_back(triangle);
                est_wall.triangles = MergeTriangleArray(est_wall.triangles, arr);

                already_wall = true;
                break;
            }

            if (!already_wall)
                walls.push_back(wall);

            // Perhaps collapse the notion of "wall" from a mesh into a BB (possibly first step)
        }
    } 

    return ConvertVerboseWalls(walls);
}

