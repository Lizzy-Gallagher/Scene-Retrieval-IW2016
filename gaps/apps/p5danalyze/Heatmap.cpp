#include "R3Graphics/R3Graphics.h"
#include "IOAux.h"
#include "StatsAux.h"
#include "Drawing.h"

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>

using HeatmapMap = std::map<std::string, std::map<std::string, R2Grid*>>;

// Take collection of grids and write them
int WriteHeatmaps(HeatmapMap* heatmaps, FrequencyStats freq_stats,
        const char* output_grid_directory, bool print_verbose)
{
    std::ofstream stats_categories_file;
    stats_categories_file.open("stats/categories.csv");
    stats_categories_file << "category,count\n"; 

    std::ofstream stats_pairs_file;
    stats_pairs_file.open("stats/pairs.csv");
    stats_pairs_file << "pri_cat,ref_cat,count\n"; 

    for (auto it : (*heatmaps)) {
        std::string pri_cat = it.first;
        std::map<std::string, R2Grid*> map = it.second;
        stats_categories_file << pri_cat.c_str() << "," << (*freq_stats.cat_count)[pri_cat] << "\n";

        for (auto it2 : map) {
            std::string ref_cat = it2.first;
            R2Grid* grid        = it2.second;

            char output_filename[1024];
            sprintf(output_filename, "%s/%s___%s.grd", output_grid_directory, 
                    pri_cat.c_str(), ref_cat.c_str());

            stats_pairs_file << pri_cat.c_str() << "," << ref_cat.c_str() << "," << (*freq_stats.pair_count)[pri_cat][ref_cat] << "\n"; 
            if (!WriteGrid(grid, output_filename, print_verbose)) return 0;
        }
    }

    stats_categories_file.close();
    stats_pairs_file.close();
    return 1;
}

void UpdateHeatmaps(R3Scene* scene, std::vector<R3SceneNode*> objects,
        HeatmapMap* heatmaps, int resolution, std::map<std::string, int>* cat_count, 
        Id2CatMap* id2cat)
{ 
    // Populate category->category map
    for (int i = 0; i < objects.size(); i++) {
        R3SceneNode* pri_obj = objects[i];
        std::string pri_cat = GetObjectCategory(pri_obj, id2cat);
        if (pri_cat.size() == 0) continue; 
        //if (pri_cat != only_category) continue;

        (*cat_count)[pri_cat]++;

        for (int j = 0; j < objects.size(); j++) {
            if (i == j) continue;

            R3SceneNode* ref_obj = objects[j];
            std::string ref_cat = GetObjectCategory(ref_obj, id2cat);
            if (ref_cat.size() == 0) continue;

            if ((*heatmaps)[pri_cat].count(ref_cat) == 0)
                (*heatmaps)[pri_cat][ref_cat] = new R2Grid(resolution, resolution); 
        }

        if ((*heatmaps)[pri_cat]["wall"] == 0)
            (*heatmaps)[pri_cat]["wall"] = new R2Grid(resolution, resolution);
    }
}

void CalcHeatmaps( R3SceneNode* pri_obj, R3SceneNode* ref_obj, std::string ref_cat,
        std::string pri_cat, Id2CatMap* id2cat, HeatmapMap* heatmaps, 
        XformValues values, double threshold, int pixels_to_meters) 
{
    // Draw objects
    R2Grid *grid = (*heatmaps)[pri_cat][ref_cat];

    R3Vector dist3d = (ref_obj->Centroid() - pri_obj->Centroid());
    R2Vector dist = R2Vector(dist3d.X(), dist3d.Y());
    values.dist = &dist; 

    // Only draw object close enough
    if (dist.Length() < threshold)
        Draw(ref_obj, grid, values, pixels_to_meters);
}
