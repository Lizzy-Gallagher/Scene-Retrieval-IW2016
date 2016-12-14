#include "R3Graphics/R3Graphics.h"
#include "IOAux.h"
#include "Drawing.h"
#include "Mode.h"

#include <string>
#include <map>
#include <math.h>
#include <vector>
#include <iostream>
#include <fstream>

// primary object -> secondary object - R2Grid*
using HeatmapMap = std::map<std::string, std::map<std::string, R2Grid*> >;

char* parseData(Mode mode, const char* output_directory, const char* data) {
    char* directory = new char[1024];
    if (mode == All) {
        sprintf(directory, "%s", output_directory);
    } else if (mode == SceneByScene) {
        sprintf(directory, "%s/%s", output_directory, data);
    } else if (mode == RoomByRoom) {
        std::string s(data);
        std::string delimiter = "|";
        std::string scene = s.substr(0, s.find(delimiter));
        s.erase(0, s.find(delimiter) + delimiter.length());
        std::string room_num = s.substr(0, s.find(delimiter));
        
        sprintf(directory, "%s/%s/%s", output_directory, scene.c_str(), room_num.c_str());
    }

    return directory;
}

int WriteHeatmap (R2Grid *grid, std::string primary_cat, std::string secondary_cat, 
    const char* output_directory, Mode mode, const char* data) {
    
    // Create Output directory
    char* directory = parseData(mode, output_directory, data);
    CreateDirectory(directory);

    // Image filename
    char img_filename[1024];
    sprintf(img_filename, "%s/%s___%s.jpg", directory, 
        primary_cat.c_str(), secondary_cat.c_str());

    // return status
    return grid->Write(img_filename);
}

// Take collection of grids and write them
int WriteHeatmaps(HeatmapMap* heatmaps, const char* output_directory, 
        Mode mode, const char* data)
{
    for (auto it : (*heatmaps)) {
        std::string pri_cat = it.first;
        std::map<std::string, R2Grid*> map = it.second;

        for (auto it2 : map) {
            std::string sec_cat = it2.first;
            R2Grid* grid        = it2.second;

            if (!WriteHeatmap(grid, pri_cat, sec_cat, output_directory, mode, data)) {
                fprintf(stderr, "Failure to write image.\n");
                return 0;
            }
        }
    }

    return 1;
}


// Just for ObjectByObject
int WriteHeatmap(R2Grid* heatmap, Mode mode, std::string scene_id, 
    int floor_num, int room_num, int primary_object_num, int secondary_object_num,
    const char* output_directory) {
    
    // Write Image
    char img_directory[1024];
    sprintf(img_directory, "%s/%s/%d/%d/%d", output_directory, 
            scene_id.c_str(), floor_num, room_num, primary_object_num);
    CreateDirectory(img_directory);
    
    char img_filename[1024];
    sprintf(img_filename, "%s/%d.jpeg", img_directory, secondary_object_num);

    heatmap->Write(img_filename);

    return 1;
}


void CalcHeatmapsByObject(R3SceneNode* primary_obj, R3SceneNode* secondary_obj,
    XformValues values, double threshold, int pixels_to_meters, std::string scene_id, 
    int floor_num, int room_num, int primary_object_num, int secondary_object_num,
    const char* output_directory) {
    
    R3Vector dist3d = (secondary_obj->Centroid() - primary_obj->Centroid());
    R2Vector dist = R2Vector(dist3d.X(), dist3d.Y());
    values.dist = &dist; 

    int resolution = pixels_to_meters * (2 * threshold - 1);
    
    if (fabs(dist.Length()) < threshold) {
        R2Grid *heatmap = new R2Grid(resolution, resolution);
        Draw(secondary_obj, heatmap, values, pixels_to_meters);
        WriteHeatmap(heatmap, ObjectByObject, scene_id, floor_num, 
            room_num, primary_object_num, secondary_object_num, output_directory);
    }
}

void CalcHeatmaps(R3SceneNode* pri_obj, R3SceneNode* sec_obj, std::string sec_cat,
    std::string pri_cat, HeatmapMap* heatmaps, XformValues values, 
    double threshold, int pixels_to_meters) 
{
    R3Vector dist3d = (sec_obj->Centroid() - pri_obj->Centroid());
    R2Vector dist = R2Vector(dist3d.X(), dist3d.Y());
    values.dist = &dist; 

    int resolution = pixels_to_meters * (2 * threshold - 1);

    // Only draw object close enough
    if (fabs(dist.Length()) < threshold) {
        if ((*heatmaps)[pri_cat].count(sec_cat) == 0)
            (*heatmaps)[pri_cat][sec_cat] = new R2Grid(resolution, resolution); 
    
        // Draw objects
        R2Grid *grid = (*heatmaps)[pri_cat][sec_cat];
        Draw(sec_obj, grid, values, pixels_to_meters);
    }
}
