// This is the header file for the Heatmap functionality

#ifndef HEATMAP_FUNC
#define HEATMAP_FUNC

#include "R3Graphics/R3Graphics.h"
#include "StatsAux.h"

#include <string>
#include <map>
#include <vector>

using HeatmapMap = std::map<std::string, std::map<std::string, R2Grid*>>;

int WriteHeatmaps(HeatmapMap* heatmaps, FrequencyStats freq_stats,
        const char* output_grid_directory, bool print_verbose);
void UpdateHeatmaps(R3Scene* scene, std::vector<R3SceneNode*> objects, 
        HeatmapMap* heatmaps, int resolution, std::map<std::string, int>* cat_count, 
        Id2CatMap* id2cat);

#endif
