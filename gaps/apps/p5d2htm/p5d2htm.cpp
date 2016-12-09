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
#include "Mode.h"

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <time.h>

// Program variables
static const char *input_data_directory;
static const char *output_grid_directory;
static const char *output_img_directory;
static int print_verbose = 0;
static int print_debug = 0;

// Stats Logging
FrequencyStats freq_stats;

// For later use
static std::map<std::string, std::string> id2cat;
static std::vector<std::string> project_ids;

static int meters_of_context = 2; // default
static int pixels_to_meters = 15; // default
static int resolution;
static int threshold;

clock_t start, finish;

Mode mode = RoomByRoom;

HeatmapMap heatmaps = HeatmapMap(); 


static void setMode(char *mode_str) {
    if (!strcmp(mode_str, "scene")) {
        mode = SceneByScene;
    } else {
        fprintf(stderr, "Did not recognize mode: %s", mode_str);
        exit(1);
    }
}

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
            else if (!strcmp(*argv, "-mode")) { argc--; argv++; setMode(*argv); }
            else { 
                fprintf(stderr, "Invalid program argument: %s", *argv); 
                exit(1); 
            }
            argv++; argc--;
        }
        else {
            if (!input_data_directory) input_data_directory = *argv;
            else if (!output_grid_directory) output_grid_directory = *argv;
            else if (!output_img_directory) output_img_directory = *argv;
            else { fprintf(stderr, "Invalid program argument: %s", *argv); exit(1); }
            argv++; argc--;
        }
    }

    // Check filenames
    if (!output_grid_directory || !output_img_directory || !input_data_directory) {
        fprintf(stderr, "Usage: p5d2htm inputdatadirectory"
                "outputgriddirectory outputimgdirectory [-mode mode]"
                "[-debug] [-v] [data-directory dir] [-ptm num] [-context num]\n");
        return 0;
    }

    resolution = pixels_to_meters * (2 * meters_of_context);
    threshold = meters_of_context + 1;

    // Return OK status 
    return 1;
}


static int Update(R3Scene *scene)
{
    if (mode == RoomByRoom) {
        R3SceneNode* root = scene->Node(0); 

        const char* name = root->Name();
        size_t prefix_len = strlen("Project#");
        name = name + prefix_len;

        std::vector<R3SceneNode*> outdoor_objects;

        for (int f = 0; f < root->NChildren(); f++) { // Floor
            R3SceneNode* floor = root->Child(f);
            for (int r = 0; r < floor->NChildren(); r++) { // Rooms
                R3SceneNode* node = floor->Child(r);
                if (strstr(node->Name(), "Object")) { // Outdoor object
                    outdoor_objects.push_back(node);
                } else { // Room
                    // Ignore "Ground"
                    P5DRoom* room = (P5DRoom*) node->Data();
                    if (!strstr(room->className, "Room")) continue;

                    for (int i = 0; i < node->NChildren(); i++) { // Objects
                        R3SceneNode* primary_obj = node->Child(i);
                        std::string primary_cat = GetObjectCategory(primary_obj, &id2cat);
                        if (primary_cat.size() == 0) continue;
        
                        XformValues values = CreateXformValues(primary_obj, resolution);

                        for (int j = 0; j < node->NChildren(); j++) {
                            if (i == j) continue;
            
                            R3SceneNode* secondary_obj = node->Child(j);
                            std::string secondary_cat = GetObjectCategory(secondary_obj, &id2cat);
                            if (secondary_cat.size() == 0) continue;

                            CalcHeatmaps(primary_obj, secondary_obj, secondary_cat, 
                                    primary_cat, &heatmaps, values, threshold, 
                                    pixels_to_meters, freq_stats.pair_count);
                        
                        }
                    }
                }
            
                std::string name_str(name);
                std::string delim_str("|");
                std::string room_num_str = std::to_string(r);
                std::string data = name_str + delim_str + room_num_str;
                WriteHeatmaps(&heatmaps, freq_stats, output_grid_directory, 
                    output_img_directory, print_verbose, mode, data.c_str());
                heatmaps = HeatmapMap(); // clear it
            }
        }
        return 1;
    }

    SceneNodes nodes = GetSceneNodes(scene);
    fprintf(stderr, "\t- Located Objects : %lu\n", nodes.objects.size());
    
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

            CalcHeatmaps(pri_obj, sec_obj, sec_cat, pri_cat, &heatmaps, 
                    values, threshold, pixels_to_meters, freq_stats.pair_count);
        }

        // Draw walls
       /* 
        for (int w = 0; w < nodes.walls.size(); w++) {
            R3SceneNode* wall_node = nodes.walls[w];
            Wall* ref_wall = nodes.walls[w];

            CalcHeatmaps(pri_obj, wall_node, "wall", pri_cat, &id2cat, &heatmaps,
            values, threshold, pixels_to_meters);
        }
        */
    }

    // Return success
    return 1;
}

int main(int argc, char **argv)
{
    // Parse program arguments
    if (!ParseArgs(argc, argv)) exit(1);

    project_ids = LoadProjectIds();
    id2cat = LoadIdToCategoryMap();

    // Create the output directory (if it does not exist)
    CreateDirectory(output_grid_directory);
    CreateDirectory(output_img_directory);

    int failures = 0;
    for (int i = 0; i < 3; i++) {
        std::string project_id = project_ids[i];

        start = clock();
        fprintf(stderr, "Working on ... %s (%d) \n", project_id.c_str(), i); 

        // Allocate new scene
        R3Scene *scene = new R3Scene();
        if (!scene) { failures++; continue; }
 
        // Parse Planner5D file
        char project_path[1024];
        sprintf(project_path, "%s/projects_room/%s/project.json", 
                input_data_directory, project_id.c_str());
        if (!scene->MyReadPlanner5DFile(project_path)) { exit(1); failures++; continue; }
        
        // Update the heatmaps
        if (!Update(scene)) { failures++; continue; }
        fprintf(stderr, "After Update.\n");
        
        if (mode == SceneByScene) {
            WriteHeatmaps(&heatmaps, freq_stats, output_grid_directory, 
                output_img_directory, print_verbose, mode, project_id.c_str());
            heatmaps = HeatmapMap(); // clear it
        }

        double task_time = (double)(clock() - start) / CLOCKS_PER_SEC;
        fprintf(stderr, "\t- Completed in : %f sec\n", task_time);
    }

    fprintf(stderr, "\n-- Failures: %d---\n", failures);
    fprintf(stderr, "\n--- Finished. ---\n");

    if (mode == All)
        WriteHeatmaps(&heatmaps, freq_stats, output_grid_directory, 
                output_img_directory, print_verbose, mode, NULL);

    // Return success 
    return 0;
}




