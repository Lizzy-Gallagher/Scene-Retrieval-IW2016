// This is the header file for the Heatmap functionality

#ifndef HEATMAP_FUNC
#define HEATMAP_FUNC

#include "R3Graphics/R3Graphics.h"
#include "Drawing.h"
#include "P5DAux.h"
#include "Mode.h"

#include <string>
#include <map>
#include <vector>

using HeatmapMap = std::map<std::string, std::map<std::string, R2Grid*> >;

int WriteGrid(R2Grid *grid, std::string primary_cat, std::string secondary_cat, 
        Mode mode, const char* data);

int WriteImage (R2Grid *grid, std::string primary_cat, std::string secondary_cat, 
        const char* output_img_directory, Mode mode, const char* data);

int WriteHeatmaps(HeatmapMap* heatmaps, const char* output_img_directory, 
        Mode mode, const char* data);

void CalcHeatmaps( R3SceneNode* pri_obj, R3SceneNode* ref_obj, std::string ref_cat, 
        std::string pri_cat, HeatmapMap* heatmaps, XformValues values, 
        double threshold, int pixels_to_meters);

void CalcHeatmapsByObject(R3SceneNode* primary_obj, R3SceneNode* secondary_obj,
    XformValues values, double threshold, int pixels_to_meters, std::string scene_id, 
    int floor_num, int room_num, int primary_object_num, int secondary_object_num,
    const char* output_img_directory);

#endif
