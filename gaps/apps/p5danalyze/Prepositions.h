// This is the header file for the Preposition Analysis


#ifndef PREPS
#define PREPS

#include "R3Graphics/R3Graphics.h"
#include "IOAux.h"
#include "StatsAux.h"
#include "P5DAux.h"

#include <map>
#include <string>
#include <vector>

/* Adding a new preposition? 
 *  - Start here
 *  - In Prepositions.cpp, add to constants
 */

enum Prep {
    PREP_FRONTSIDE   = 0,
    PREP_BACKSIDE    = 1,
    PREP_ABOVE       = 2,
    PREP_BELOW       = 3,
    PREP_RIGHTSIDE   = 4,
    PREP_LEFTSIDE    = 5,
    PREP_ON_TOP      = 6,
    PREP_NEAR        = 7,
    PREP_WITHIN      = 8,
    NUM_PREPOSITIONS = 9,
};

using PrepositionStats = std::vector<int>;

struct PrepRegion {
    const char* name;
    int prep;
    R3Box region;
};

using PrepMap = std::map<std::string, std::map<std::string, PrepositionStats>>;
using PrepRegionMap = std::map<int, PrepRegion>;

// Needs a new name
void PopulatePrepMap(R3Scene* scene, std::vector<R3SceneNode*> objects,
        PrepMap* prep_map, FrequencyStats freq_stats, Id2CatMap* id2cat);

// Needs a new name
PrepRegionMap InitPrepRegions(R3SceneNode* node, int meters_of_context);

// Needs a new name
void CalcPrepositions(R3SceneNode* pri_obj, Wall* ref_wall, std::string pri_cat, 
        PrepRegionMap prep_region_map, PrepMap* prep_map, int meters_of_context);
void CalcPrepositions(R3SceneNode* pri_obj, R3SceneNode* ref_obj, std::string pri_cat, 
        std::string ref_cat, PrepRegionMap prep_region_map, PrepMap* prep_map, 
        int meters_of_context, FrequencyStats& freq_stats);

// needs a new name
int WritePrepMap(PrepMap* prepmap, FrequencyStats freq_stats);

#endif
