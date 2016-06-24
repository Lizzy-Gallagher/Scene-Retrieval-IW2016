// This is the header file for the Heatmap functionality

#ifndef HEATMAP_FUNC
#define HEATMAP_FUNC

#include "R3Graphics/R3Graphics.h"
#include "StatsAux.h"
#include "R2Aux.h"
#include "P5DAux.h"

#include <string>
#include <map>
#include <vector>

using HeatmapMap = std::map<std::string, std::map<std::string, R2Grid*>>;

int WriteHeatmaps(HeatmapMap* heatmaps, FrequencyStats freq_stats,
        const char* output_grid_directory, bool print_verbose);
void UpdateHeatmaps(R3Scene* scene, std::vector<R3SceneNode*> objects, 
        HeatmapMap* heatmaps, int resolution, std::map<std::string, int>* cat_count, 
        Id2CatMap* id2cat);
void CalcHeatmaps( R3SceneNode* pri_obj, R3SceneNode* ref_obj, std::string ref_cat, 
        std::string pri_cat, Id2CatMap* id2cat, HeatmapMap* heatmaps, 
        DrawingValues values, double threshold, int pixels_to_meters);

#endif
