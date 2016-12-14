#include "R3Graphics/R3Graphics.h"
#include "IOAux.h"
#include "StatsAux.h"
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

int WriteImage (R2Grid *grid, std::string primary_cat, std::string secondary_cat, 
        const char* output_img_directory, Mode mode, const char* data) {
    const char* directory = output_img_directory; // default
    
    // If in scene-by-scene mode, create directory for scene
    if (mode == SceneByScene) {
        char scene_directory[1024];
        sprintf(scene_directory, "%s/%s", output_img_directory, data);
        CreateDirectory(scene_directory);
        directory = scene_directory;
    } else if (mode == RoomByRoom) {
        std::string s(data);
        std::string delimiter = "|";
        std::string scene = s.substr(0, s.find(delimiter));
        s.erase(0, s.find(delimiter) + delimiter.length());
        std::string room_num = s.substr(0, s.find(delimiter));

        char room_directory[1024];
        sprintf(room_directory, "%s/%s/%s", output_img_directory, 
                scene.c_str(), room_num.c_str());
        CreateDirectory(room_directory);
        directory = room_directory;
    }
 
    
    char img_filename[1024];
    sprintf(img_filename, "%s/%s___%s.jpg", directory, 
        primary_cat.c_str(), secondary_cat.c_str());

    // return status
    return grid->Write(img_filename);

}

// Take collection of grids and write them
int WriteHeatmaps(HeatmapMap* heatmaps, FrequencyStats freq_stats,
        const char* output_img_directory, bool print_verbose, Mode mode, const char* data)
{
    std::ofstream stats_categories_file;
    stats_categories_file.open("output/stats/categories.csv");
    stats_categories_file << "category,count\n"; 

    std::ofstream stats_pairs_file;
    stats_pairs_file.open("output/stats/pairs.csv");
    stats_pairs_file << "pri_cat,sec_cat,count\n"; 

    for (auto it : (*heatmaps)) {
        std::string pri_cat = it.first;
        std::map<std::string, R2Grid*> map = it.second;
        stats_categories_file << pri_cat.c_str() << "," << 
            (*freq_stats.cat_count)[pri_cat] << "\n";

        for (auto it2 : map) {
            std::string sec_cat = it2.first;
            R2Grid* grid        = it2.second;

            stats_pairs_file << pri_cat.c_str() << "," << sec_cat.c_str() << 
                "," << (*freq_stats.pair_count)[pri_cat][sec_cat] << "\n"; 

            if (!WriteImage(grid, pri_cat, sec_cat, output_img_directory, mode, data)) {
                fprintf(stderr, "Failure to write image.\n");
                return 0;
            }
        }
    }

    stats_categories_file.close();
    stats_pairs_file.close();
    return 1;
}


// Just for ObjectByObject
int WriteHeatmap(R2Grid* heatmap, Mode mode, std::string scene_id, 
    int floor_num, int room_num, int primary_object_num, int secondary_object_num,
    const char* output_img_directory) {
    
    // Write Image
    char img_directory[1024];
    sprintf(img_directory, "%s/%s/%d/%d/%d", output_img_directory, 
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
    const char* output_img_directory) {
    
    R3Vector dist3d = (secondary_obj->Centroid() - primary_obj->Centroid());
    R2Vector dist = R2Vector(dist3d.X(), dist3d.Y());
    values.dist = &dist; 

    int resolution = pixels_to_meters * (2 * threshold - 1);
    
    if (fabs(dist.Length()) < threshold) {
        R2Grid *heatmap = new R2Grid(resolution, resolution);
        Draw(secondary_obj, heatmap, values, pixels_to_meters);
        WriteHeatmap(heatmap, ObjectByObject, scene_id,
                floor_num, room_num, primary_object_num, secondary_object_num,
                output_img_directory);
    }
}

void CalcHeatmaps( R3SceneNode* pri_obj, R3SceneNode* sec_obj, std::string sec_cat,
    std::string pri_cat, HeatmapMap* heatmaps, XformValues values, 
    double threshold, int pixels_to_meters, PairMap* pair_count) 
{
    R3Vector dist3d = (sec_obj->Centroid() - pri_obj->Centroid());
    R2Vector dist = R2Vector(dist3d.X(), dist3d.Y());
    values.dist = &dist; 

    int resolution = pixels_to_meters * (2 * threshold - 1);

    // Only draw object close enough
    if (fabs(dist.Length()) < threshold) {
        // Count pair 
        (*pair_count)[pri_cat][sec_cat]++;

        if ((*heatmaps)[pri_cat].count(sec_cat) == 0)
            (*heatmaps)[pri_cat][sec_cat] = new R2Grid(resolution, resolution); 
    
        // Draw objects
        R2Grid *grid = (*heatmaps)[pri_cat][sec_cat];
        Draw(sec_obj, grid, values, pixels_to_meters);
    }
}
