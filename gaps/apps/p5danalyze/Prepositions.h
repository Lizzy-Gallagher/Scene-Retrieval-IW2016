// This is the header file for the Preposition Analysis


#ifndef PREPS
#define PREPS

#include "R3Graphics/R3Graphics.h"
#include "IOAux.h"
#include "StatsAux.h"

#include <map>
#include <string>
#include <vector>

enum Prep {
    PREP_FRONTSIDE   = 0,
    PREP_BACKSIDE    = 1,
    PREP_ABOVE       = 2,
    PREP_BELOW       = 3,
    PREP_RIGHTSIDE   = 4,
    PREP_LEFTSIDE    = 5,
    NUM_PREPOSITIONS = 6,
};

struct PrepositionStats {
    int frontside;
    int backside;
    int above;
    int below; 
    int leftside;
    int rightside;
};

struct PrepRegion {
    const char* name;
    int prep;
    R3Box region;
};

using PrepMap = std::map<std::string, std::map<std::string, PrepositionStats>>;
using PrepRegionMap = std::map<int, PrepRegion>;


void UpdatePrepositions(R3Scene* scene, std::vector<R3SceneNode*> objects,
        PrepMap* prep_map, FrequencyStats freq_stats, Id2CatMap* id2cat);
std::map<int, PrepRegion> InitPrepositions(R3SceneNode* node, int meters_of_context);
void CalcPrepositions(R3SceneNode* pri_obj, R3SceneNode* ref_obj, std::string pri_cat, std::string ref_cat,
        Id2CatMap* id2cat, PrepRegionMap prep_region_map, PrepMap* prep_map, int meters_of_context,
        FrequencyStats& freq_stats);
int WritePrepositions(PrepMap* prepmap, FrequencyStats freq_stats);

#endif
