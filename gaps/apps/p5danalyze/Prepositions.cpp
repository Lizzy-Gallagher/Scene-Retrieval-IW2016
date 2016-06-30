#include "R3Graphics/R3Graphics.h"
#include "StatsAux.h"
#include "Prepositions.h"
#include "Drawing.h"
#include "IOAux.h"

#include <map>
#include <string>
#include <vector>

// TUNING
double on_top_threshold = 0.1;
double nearness_threshold = 1.5; // TODO: this is fraction of original size, need to add math to make this "meters"
// ... perhaps

const char* prep_names[] = {
    "FRONTSIDE",
    "BACKSIDE",
    "ABOVE",
    "BELOW",
    "RIGHTSIDE",
    "LEFTSIDE",
    "ON_TOP",
    "NEAR",
    "WITHIN",
};

/* Debug */

void PrintSide(R3Box& region, const char* name) {
    fprintf(stdout, "\n\t%s \n\t\t(%f, %f, %f) (%f, %f, %f) \n\t\t(%f, %f, %f) (%f, %f, %f)\n",
           name,
           region.Corner(0).X(), region.Corner(0).Y(), region.Corner(0).Z(),
           region.Corner(1).X(), region.Corner(1).Y(), region.Corner(1).Z(),
           region.Corner(5).X(), region.Corner(5).Y(), region.Corner(5).Z(),
           region.Corner(6).X(), region.Corner(6).Y(), region.Corner(6).Z());
}

void PrintBox(R3Box region, const char* name) {
    fprintf(stderr, "\n\t%s \n\t\t(%f, %f, %f) (%f, %f, %f) \n\t\t(%f, %f, %f) (%f, %f, %f)"
                    "\n\t\t(%f, %f, %f) (%f, %f, %f) \n\t\t(%f, %f, %f) (%f, %f, %f)\n",
           name,
           region.Corner(0).X(), region.Corner(0).Y(), region.Corner(0).Z(),
           region.Corner(1).X(), region.Corner(1).Y(), region.Corner(1).Z(),
           region.Corner(2).X(), region.Corner(2).Y(), region.Corner(2).Z(),
           region.Corner(3).X(), region.Corner(3).Y(), region.Corner(3).Z(),
           region.Corner(4).X(), region.Corner(4).Y(), region.Corner(4).Z(),
           region.Corner(5).X(), region.Corner(5).Y(), region.Corner(5).Z(),
           region.Corner(6).X(), region.Corner(6).Y(), region.Corner(6).Z(),
           region.Corner(7).X(), region.Corner(7).Y(), region.Corner(7).Z());
}

/* Helpers */

PrepRegion CalcPrepRegion(R3Box bb, int preposition, int meters_of_context) {
    R3Box region;
    switch (preposition) {
        case PREP_FRONTSIDE: region = bb.Side(RN_HY_SIDE); break;
        case PREP_BACKSIDE:  region = bb.Side(RN_LY_SIDE); break;
        case PREP_ABOVE:     region = bb.Side(RN_HZ_SIDE); break;
        case PREP_BELOW:     region = bb.Side(RN_LZ_SIDE); break;
        case PREP_RIGHTSIDE: region = bb.Side(RN_HX_SIDE); break;
        case PREP_LEFTSIDE:  region = bb.Side(RN_LX_SIDE); break;
        case PREP_ON_TOP:    region = bb.Side(RN_HZ_SIDE); break;
        case PREP_NEAR:      region = R3Box(bb); break;
        case PREP_WITHIN:    region = R3Box(bb); break;
    }
    
    R3Point min = region.Min(); 
    R3Point max = region.Max();

    switch (preposition) {
        case PREP_FRONTSIDE: max.SetY(max.Y() + meters_of_context); break;
        case PREP_BACKSIDE:  min.SetY(min.Y() - meters_of_context); break;
        case PREP_ABOVE:     max.SetZ(max.Z() + meters_of_context); break;
        case PREP_BELOW:     min.SetZ(min.Z() - meters_of_context); break;
        case PREP_RIGHTSIDE: max.SetX(max.X() + meters_of_context); break;
        case PREP_LEFTSIDE:  min.SetX(min.X() - meters_of_context); break;
        case PREP_ON_TOP:    max.SetZ(max.Z() + on_top_threshold);  break;
        case PREP_NEAR:      region.Inflate(nearness_threshold); break;
        case PREP_WITHIN:    break; // do nothing
    }

    PrepRegion pr = { prep_names[preposition], preposition, R3Box(min, max) };
    return pr;
}


void UpdateStats(PrepositionStats& prep_stats, int preposition) {  
    ++prep_stats[preposition];
}

R3Box* CalcIntersection(R3Box b1, R3Box b2) {
    R3Box* area_of_intersection = NULL;

    // Check if any CalcIntersection
    if (b1.XMax() <= b2.XMin() || b2.XMax() <= b1.XMin() ||
        b1.YMax() <= b2.YMin() || b2.YMax() <= b1.YMin() ||
        b1.ZMax() <= b2.ZMin() || b2.ZMax() <= b1.ZMin())
        return area_of_intersection;

    // Now compute area of intersection
    float xMin = std::max(b1.XMin(), b2.XMin());
    float yMin = std::max(b1.YMin(), b2.YMin());
    float zMin = std::max(b1.ZMin(), b2.ZMin());

    float xMax = std::min(b1.XMax(), b2.XMax());
    float yMax = std::min(b1.YMax(), b2.YMax());
    float zMax = std::min(b1.ZMax(), b2.ZMax());

    area_of_intersection = new R3Box(xMin, yMin, zMin, xMax, yMax, zMax);
    return area_of_intersection;
}

/* Public Methods */

void PopulatePrepMap(R3Scene* scene, std::vector<R3SceneNode*> objects,
        PrepMap* prep_map, FrequencyStats freq_stats, Id2CatMap* id2cat)
{
    // Populate category->category map
    for (int i = 0; i < objects.size(); i++) {
        R3SceneNode* pri_node = objects[i];
        std::string pri_cat = GetObjectCategory(pri_node, id2cat);
        if (pri_cat.size() == 0) continue; 
        //if (pri_cat != only_category) continue;

        (*freq_stats.cat_count)[pri_cat]++;

        for (int j = 0; j < objects.size(); j++) {
            if (i == j) continue;

            R3SceneNode* ref_node = objects[j];
            std::string ref_cat = GetObjectCategory(ref_node, id2cat);
            if (ref_cat.size() == 0) continue;
            
            if ((*prep_map)[pri_cat].count(ref_cat) == 0)
                (*prep_map)[pri_cat][ref_cat] = PrepositionStats(NUM_PREPOSITIONS); 

        }

        if ((*prep_map)[pri_cat].count("wall") == 0)
            (*prep_map)[pri_cat]["wall"] = PrepositionStats(NUM_PREPOSITIONS);
    }
}

PrepRegionMap InitPrepRegions(R3SceneNode* node, int meters_of_context) {
    PrepRegionMap prep_region_map;

    R3Box bb = node->BBox();

    // Transform the BB to standardize
    XformValues values = CreateXformValues(node);
    R3Affine world_to_grid_xform = PrepareWorldToGridXform(node->Centroid(), values);
    bb.Transform(world_to_grid_xform);

    // Calculate the "region" of each preposition
    for (int prep_int = 0; prep_int < NUM_PREPOSITIONS; ++prep_int)
         prep_region_map[prep_int] = CalcPrepRegion(bb, prep_int, meters_of_context);

    return prep_region_map;
}

void CalcPrepositions(R3SceneNode* pri_obj, R3SceneNode* ref_obj, std::string pri_cat, 
        std::string ref_cat, Id2CatMap* id2cat, PrepRegionMap prep_region_map, 
        PrepMap* prep_map, int meters_of_context, FrequencyStats& freq_stats) {
    R3Box ref_bb = ref_obj->BBox();

    // Create and Apply transform to put nearby region
    XformValues values = CreateXformValues(pri_obj);
    R3Vector dist3d = (ref_obj->Centroid() - pri_obj->Centroid());
    values.dist3D = &dist3d;  // TODO: fix 
    R3Affine world_to_grid_xform = PrepareWorldToGridXform(ref_obj->Centroid(), values);
    ref_bb.Transform(world_to_grid_xform);

    if (dist3d.Length() > meters_of_context)
        return;

    ++(*freq_stats.pair_count)[pri_cat][ref_cat]; 

    for (int i = 0; i < NUM_PREPOSITIONS; ++i) {
        PrepRegion pr = prep_region_map[i];
        R3Box region = pr.region;

        R3Box* result = CalcIntersection(region, ref_bb);
        if (result == NULL)
            continue;
        
        float volume_of_overlap = result->Volume() / ref_bb.Volume();
        
        //if (volume_of_overlap < 0.5)
        //    continue;

        PrepositionStats prep_stats = (*prep_map)[pri_cat][ref_cat];
        UpdateStats(prep_stats, i);
        (*prep_map)[pri_cat][ref_cat] = prep_stats;
    }
}


int WritePrepMap(PrepMap* prepmap, FrequencyStats freq_stats) {
    PrepMap prep_map = *prepmap;
    for (auto it : prep_map) {
        std::string pri_cat = it.first;
        std::map<std::string, PrepositionStats> map = it.second;

        fprintf(stdout, "[%s]\n", pri_cat.c_str());
        for (auto it2 : map) {
            std::string ref_cat = it2.first;
            PrepositionStats prep_stats = it2.second;

            double div = (double) ((*freq_stats.pair_count)[pri_cat][ref_cat]);
            if (div <= 0.0)
                continue;

            bool all_zero = true;

            // Cleaner priting code
            double probabilities[NUM_PREPOSITIONS]; 
            for (int i = 0; i < NUM_PREPOSITIONS; i++) {
                probabilities[i] = prep_stats[i] / div; 
                if (prep_stats[i] > 0)
                    all_zero = false;
            }

            if (all_zero)
                continue;
            
            fprintf(stdout, "\t[%s] ", ref_cat.c_str());
            for (int i = 0; i < NUM_PREPOSITIONS; i++) {
                fprintf(stdout, "%s : %f (%d), ", prep_names[i], probabilities[i], prep_stats[i]);
            }
            fprintf(stdout, "\n");
        }
    }

    // Return success
    return 1;
}
