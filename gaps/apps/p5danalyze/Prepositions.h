// This is the header file for the Preposition Analysis


#ifndef PREPS
#define PREPS

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

enum Prep {
    PREP_FRONTSIDE   = 0,
    PREP_BACKSIDE    = 1,
    PREP_ABOVE       = 2,
    PREP_BELOW       = 3,
    PREP_RIGHTSIDE   = 4,
    PREP_LEFTSIDE    = 5,
    NUM_PREPOSITIONS = 6,
};

struct PrepRegion {
    const char* name;
    int prep;
    R3Box region;
};

/*class PrepRegion {
    public:
        // Constructor Functions
        PrepRegion(const char* name, int prep, R3Box region);
        PrepRegion(const PrepRegion& prep_region);

        // Access Functions
        const char *Name(void);
        int Preposition(void);
        R3Box Region(void);

    private:
        const char* name;
        int prep;
        R3Box region;
};

inline PrepRegion::PrepRegion(const char* name, int prep, R3Box region) : name(name), prep(prep), region(region)
{
    fprintf(stdout, "INITING COPY!!\n");
}

inline PrepRegion::PrepRegion(const PrepRegion& prep_region) : name(prep_region.name), prep(prep_region.prep), region(R3Box(prep_region.region))
{}

// Inline Functions
inline const char* PrepRegion::
Name(void)
{
    return name;
}

inline int PrepRegion::
Preposition(void)
{
    return prep;
}

inline R3Box PrepRegion::
Region(void)
{
    return region;
}*/


void UpdatePrepositions(R3Scene* scene, std::vector<R3SceneNode*> objects,
        PrepMap* prep_map, FrequencyStats freq_stats, Id2CatMap* id2cat);
std::map<int, PrepRegion> InitPrepositions(R3SceneNode* node);
void CalcPrepositions(R3SceneNode* pri_obj, R3SceneNode* ref_obj, std::string pri_cat, std::string ref_cat,
        Id2CatMap* id2cat, std::map<int, PrepRegion> prep_region_map);

#endif
