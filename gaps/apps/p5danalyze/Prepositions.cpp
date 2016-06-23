#include "R3Graphics/R3Graphics.h"
#include "StatsAux.h"
#include "Prepositions.h"

#include <map>
#include <string>
#include <vector>

using PrepMap = std::map<std::string, std::map<std::string, PrepositionStats>>;

void UpdatePrepositions(R3Scene* scene, std::vector<R3SceneNode*> objects,
        PrepMap* prep_map, FrequencyStats freq_stats, Id2CatMap* id2cat)
{
    // Populate category->category map
    for (int i = 0; i < objects.size(); i++) {
        R3SceneNode* pri_obj = objects[i];
        std::string pri_cat = GetObjectCategory(pri_obj, id2cat);
        if (pri_cat.size() == 0) continue; 
        //if (pri_cat != only_category) continue;

        (*freq_stats.cat_count)[pri_cat]++;

        for (int j = 0; j < objects.size(); j++) {
            if (i == j) continue;

            R3SceneNode* ref_obj = objects[j];
            std::string ref_cat = GetObjectCategory(ref_obj, id2cat);
            if (ref_cat.size() == 0) continue;
            
            if ((*prep_map)[pri_cat].count(ref_cat) == 0)
                (*prep_map)[pri_cat][ref_cat] = {}; 
        }

        if ((*prep_map)[pri_cat].count("wall") == 0)
            (*prep_map)[pri_cat]["wall"] = {};
    }
}

