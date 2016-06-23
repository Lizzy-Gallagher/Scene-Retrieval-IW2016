#include "R3Graphics/R3Graphics.h"
#include "P5DAux.h"

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

