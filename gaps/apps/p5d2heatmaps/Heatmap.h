// This is the header file for the Heatmap functionality

#ifndef HEATMAP_FUNC
#define HEATMAP_FUNC

#include "R3Graphics/R3Graphics.h"
#include "StatsAux.h"
#include "Drawing.h"
#include "P5DAux.h"

#include <string>
#include <map>
#include <vector>

using HeatmapMap = std::map<std::string, std::map<std::string, R2Grid*> >;

int WriteHeatmaps(HeatmapMap* heatmaps, FrequencyStats freq_stats,
        const char* output_grid_directory, const char* output_img_directory, 
        bool print_verbose);
void CalcHeatmaps( R3SceneNode* pri_obj, R3SceneNode* ref_obj, std::string ref_cat, 
        std::string pri_cat, Id2CatMap* id2cat, HeatmapMap* heatmaps, 
        XformValues values, double threshold, int pixels_to_meters, PairMap* pair_count);

#endif
