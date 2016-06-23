// This is the header file for the Preposition Analysis


#ifndef PREP
#define PREP

#include "R3Graphics/R3Graphics.h"
#include "IOAux.h"

#include <map>
#include <string>
#include <vector>

struct PrepositionStats {
    int frontside;
    int backside;
    int leftside;
    int rightside;
    int below;
    int above;
};

using PrepMap = std::map<std::string, std::map<std::string, PrepositionStats>>;

void UpdatePrepositions(R3Scene* scene, std::vector<R3SceneNode*> objects,
        PrepMap* prep_map, FrequencyStats freq_stats, Id2CatMap* id2cat);

#endif
