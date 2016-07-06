// This is the header file for working with P5D Scenes

#ifndef P5D_AUX
#define P5D_AUX

#include "R3Graphics/R3Graphics.h"
#include <vector>

struct Wall {
    // Stats for construction 
    bool isHorizon;
    bool isVert;

    float x = 0.0;
    float y = 0.0;

    R3TriangleArray* triangles;
};

using Walls = std::vector<Wall>;

struct SceneNodes {
    std::vector<R3SceneNode*> objects; 
    //std::vector<Wall*> walls;
    std::vector<Wall> walls;
};

SceneNodes GetSceneNodes(R3Scene *scene);
Walls GetWalls(R3SceneNode* room);

#endif
