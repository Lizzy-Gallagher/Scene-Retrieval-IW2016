// Source file for the scene analyzer program

// Include files 

#include "R3Graphics/R3Graphics.h"
#include "fglut/fglut.h"
#include "R3Graphics/p5d.h"

#include "P5DAux.h"
#include "Drawing.h"
#include "IOAux.h"
#include "StatsAux.h"
#include "Heatmap.h"

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <time.h>

// Program variables
static const char *input_data_directory = "data/";
static const char *output_grid_directory = NULL;
static const char *output_img_directory = NULL;
static int print_verbose = 0;
static int print_debug = 0;

// Stats Logging
FrequencyStats freq_stats;

// For later use
static std::map<std::string, std::string> id2cat;
static std::vector<std::string> project_ids;

static int meters_of_context = 2;
static int pixels_to_meters = 15;
static int resolution = pixels_to_meters * (2 * meters_of_context);

static double threshold = meters_of_context + 1;
clock_t start, finish;

HeatmapMap heatmaps = HeatmapMap(); 

static int ParseArgs(int argc, char **argv)
{
    // Parse arguments
    argc--; argv++;
    while (argc > 0) {
        if ((*argv)[0] == '-') {
            if (!strcmp(*argv, "-v")) print_verbose = 1; 
            else if (!strcmp(*argv, "-debug")) print_debug = 1; 
            else if (!strcmp(*argv, "-data_directory")) { argc--; argv++; input_data_directory = *argv; }
            else if (!strcmp(*argv, "-ptm")) { argc--; argv++; pixels_to_meters = atoi(*argv); }
            else if (!strcmp(*argv, "-context")) { argc--; argv++; meters_of_context = atoi(*argv); }

            else { 
                fprintf(stderr, "Invalid program argument: %s", *argv); 
                exit(1); 
            }
            argv++; argc--;
        }
        else {
            if (!output_grid_directory) output_grid_directory = *argv;
            else if (!output_img_directory) output_img_directory = *argv;
            else { fprintf(stderr, "Invalid program argument: %s", *argv); exit(1); }
            argv++; argc--;
        }
    }

    // Check filenames
    if (!output_grid_directory || !output_img_directory) {
        fprintf(stderr, "Usage: p5danalyze outputgriddirectory outputimgdirectory"
                "[-debug] [-v] [data-directory dir] [-ptm num] [-context num]\n");
        return 0;
    }

    // Return OK status 
    return 1;
}


static int Update(R3Scene *scene)
{
    SceneNodes nodes = GetSceneNodes(scene);
    fprintf(stderr, "\t- Located Objects : %lu\n", nodes.objects.size());

    // TODO: Unnecessary, added to CalcHeatmaps
    UpdateHeatmaps(scene, nodes.objects, &heatmaps, resolution, freq_stats.cat_count, &id2cat);
    fprintf(stderr, "\t- Updated Collection\n");

    for (int i = 0; i < nodes.objects.size(); i++) {
        R3SceneNode* pri_obj = nodes.objects[i];
        std::string pri_cat = GetObjectCategory(pri_obj, &id2cat);
        if (pri_cat.size() == 0) continue;
        XformValues values = CreateXformValues(pri_obj, resolution);

        // Draw objects
        for (int j = 0; j < nodes.objects.size(); j++) {
            if (i == j) continue;

            R3SceneNode* sec_obj = nodes.objects[j];
            std::string sec_cat = GetObjectCategory(sec_obj, &id2cat);
            if (sec_cat.size() == 0) continue;

            CalcHeatmaps(pri_obj, sec_obj, sec_cat, pri_cat, &id2cat, &heatmaps, values, threshold, pixels_to_meters, freq_stats.pair_count);
        }

        // Draw walls
       /* 
        for (int w = 0; w < nodes.walls.size(); w++) {
            R3SceneNode* wall_node = nodes.walls[w];
            Wall* ref_wall = nodes.walls[w];

            CalcHeatmaps(pri_obj, wall_node, "wall", pri_cat, &id2cat, &heatmaps, values, threshold, pixels_to_meters);
        }
        */
    }

    // Return success
    return 1;
}

void CreateDirectory(const char* dir_name) {
    char cmd[1024];
    sprintf(cmd, "mkdir -p %s", dir_name);
    system(cmd);
}


int main(int argc, char **argv)
{
    // Parse program arguments
    if (!ParseArgs(argc, argv)) exit(-1);

    // Create a vector of all project IDs
    project_ids = LoadProjectIds();

    // Create mapping of ids to object categories
    id2cat = LoadIdToCategoryMap();

    // Create the output directory (if it does not exist)
    CreateDirectory(output_grid_directory);
    CreateDirectory(output_img_directory);

    int failures = 0;
    int i = 0; 
    for (std::string project_id : project_ids) // For each project...
    {
        if (i == 5) break;

        start = clock();
        fprintf(stderr, "Working on ... %s (%d) \n", project_id.c_str(), i); 

        // Read project
        P5DProject *project = ReadProject(project_id.c_str(), print_verbose, input_data_directory);
        if (!project) { failures++; continue; }
        fprintf(stderr, "\t- Read Project\n");

        // Create scene
        R3Scene *scene = CreateScene(project, input_data_directory);
        if (!scene) { failures++; continue; }
        fprintf(stderr, "\t- Created Scene\n");

        // Update the task goal
        if (!Update(scene)) { failures++; continue; }
        fprintf(stderr, "\t- Completed in : %f sec\n", (double)(clock() - start) / CLOCKS_PER_SEC);

        i++;
    }

    fprintf(stderr, "\n-- Failures: %d---\n", failures);
    fprintf(stderr, "\n--- Finished. ---\n");

    WriteHeatmaps(&heatmaps, freq_stats, output_grid_directory, output_img_directory,
            print_verbose);

    // Return success 
    return 0;
}




