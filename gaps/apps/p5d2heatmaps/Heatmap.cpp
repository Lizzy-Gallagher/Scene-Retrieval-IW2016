#include "R3Graphics/R3Graphics.h"
#include "IOAux.h"
#include "StatsAux.h"
#include "Drawing.h"

#include <string>
#include <map>
#include <math.h>
#include <vector>
#include <iostream>
#include <fstream>

using HeatmapMap = std::map<std::string, std::map<std::string, R2Grid*> >;

// Take collection of grids and write them
int WriteHeatmaps(HeatmapMap* heatmaps, FrequencyStats freq_stats,
        const char* output_grid_directory, const char* output_img_directory,
        bool print_verbose)
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
        stats_categories_file << pri_cat.c_str() << "," << (*freq_stats.cat_count)[pri_cat] << "\n";

        for (auto it2 : map) {
            std::string sec_cat = it2.first;
            R2Grid* grid        = it2.second;

            char grid_filename[1024];
            sprintf(grid_filename, "%s/%s___%s.grd", output_grid_directory, 
                    pri_cat.c_str(), sec_cat.c_str());

            stats_pairs_file << pri_cat.c_str() << "," << sec_cat.c_str() << "," << (*freq_stats.pair_count)[pri_cat][sec_cat] << "\n"; 
            if (!WriteGrid(grid, grid_filename, print_verbose)) {
                fprintf(stderr, "Failure to write grid.\n");
            }

            char img_filename[1024];
            sprintf(img_filename, "%s/%s___%s.jpg", output_img_directory, 
                    pri_cat.c_str(), sec_cat.c_str());
            if (!WriteGrid(grid, img_filename, print_verbose)) {
                fprintf(stderr, "Failure to write grid (img).\n");
            }

        }
    }

    stats_categories_file.close();
    stats_pairs_file.close();
    return 1;
}

void CalcHeatmaps( R3SceneNode* pri_obj, R3SceneNode* sec_obj, std::string sec_cat,
        std::string pri_cat, Id2CatMap* id2cat, HeatmapMap* heatmaps, 
        XformValues values, double threshold, int pixels_to_meters, PairMap* pair_count) 
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
