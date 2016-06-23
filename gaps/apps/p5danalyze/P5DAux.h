// This is the header file for working with P5D Scenes

#ifndef P5D_AUX
#define P5D_AUX

#include "R3Graphics/R3Graphics.h"
#include <vector>

struct SceneNodes {
    std::vector<R3SceneNode*> objects; 
    std::vector<R3SceneNode*> walls;
};

SceneNodes GetSceneNodes(R3Scene *scene);

#endif
