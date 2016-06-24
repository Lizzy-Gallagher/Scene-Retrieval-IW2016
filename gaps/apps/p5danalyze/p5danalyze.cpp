// Source file for the scene analyzer program

// Include files 

#include "R3Graphics/R3Graphics.h"
#include "R3Graphics/p5d.h"
#include "fglut/fglut.h"
#include "csv.h"

#include "P5DAux.h"
#include "R2Aux.h"
#include "IOAux.h"
#include "StatsAux.h"
#include "Heatmap.h"
#include "Prepositions.h"

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <time.h>

// Tasks to complete
enum Task
{
    NOT_SET = 0,
    HEATMAPS = 1,
    INTRINSIC_PREPOSITIONS = 2,
};

// Program variables
static const char *input_data_directory = "data/";
static const char *output_grid_directory = NULL;
static int print_verbose = 0;
static int print_debug = 0;

// Stats Logging
FrequencyStats freq_stats;

// For later use
static std::map<std::string, std::string> id2cat;
static std::vector<std::string> project_ids;

static int meters_of_context = 3;
static int pixels_to_meters = 15;
static int resolution = pixels_to_meters * (2 * meters_of_context);

static double threshold = meters_of_context + 1;
clock_t start, finish;

// By Task
Task task = NOT_SET; 
HeatmapMap heatmaps = HeatmapMap(); 
PrepMap prep_map = PrepMap();

// Waiting for integration
//std::string only_category = "sofa";

/*-------------------------------------------------------------------------*/
//  Initialization Functions  
/*-------------------------------------------------------------------------*/

    static int 
ParseArgs(int argc, char **argv)
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

            // Set Task
            else if (!strcmp(*argv, "-heat")) { 
                if (task != NOT_SET) return 0;
                task = HEATMAPS; }
            else if (!strcmp(*argv, "-prep")) { 
                if (task != NOT_SET) return 0;
                task = INTRINSIC_PREPOSITIONS; } 
            //else if (!strcmp(*argv, "-category")) { argc--; argv++; only_category = *argv; }
            else { 
                fprintf(stderr, "Invalid program argument: %s", *argv); 
                exit(1); 
            }
            argv++; argc--;
        }
        else {
            if (!output_grid_directory) output_grid_directory = *argv;
            else { fprintf(stderr, "Invalid program argument: %s", *argv); exit(1); }
            argv++; argc--;
        }
    }

    // Check filenames
    if (!output_grid_directory || task == NOT_SET) {
        fprintf(stderr, "Usage: p5dview outputgriddirectory <task> [-debug] [-v]" 
                "[data-directory dir] [-ptm num] [-context num] [-category id]  \n");
        return 0;
    }

    // Return OK status 
    return 1;
}



/*-------------------------------------------------------------------------*/
//  
/*-------------------------------------------------------------------------*/

static int Update(R3Scene *scene)
{
    SceneNodes nodes = GetSceneNodes(scene);
    fprintf(stdout, "\t- Located Objects : %lu\n", nodes.objects.size());

    switch ( task ) {
        case HEATMAPS:
            UpdateHeatmaps(scene, nodes.objects, &heatmaps, resolution, freq_stats.cat_count, &id2cat);
            break;
        case INTRINSIC_PREPOSITIONS:
            UpdatePrepositions(scene, nodes.objects, &prep_map, freq_stats, &id2cat);
            break;
    } 
    fprintf(stdout, "\t- Updated Collection\n");

    // For each object, calculate a grid that identifies what it means to be
    // "on the front side of, etc
    for (int i = 0; i < nodes.objects.size(); i++) {

        R3SceneNode* pri_obj = nodes.objects[i];
        std::string pri_cat = GetObjectCategory(pri_obj, &id2cat);
        if (pri_cat.size() == 0) continue;

        // Translation constants
        float a = 0.5 * (resolution - 1) - pri_obj->Centroid().X();
        float b = 0.5 * (resolution - 1) - pri_obj->Centroid().Y();
        R2Vector translation(a, b);

        // Rotation constants
        P5DObject *p5d_obj = (P5DObject *) pri_obj->Data();
        RNAngle theta = p5d_obj->a;

        if (!strcmp(p5d_obj->className, "Door")) theta += RN_PI_OVER_TWO;
        else if (!strcmp(p5d_obj->className, "Window")) theta += RN_PI_OVER_TWO;

        bool do_fX = p5d_obj->fX;
        bool do_fY = p5d_obj->fY;

        DrawingValues values = { translation, theta, do_fX, do_fY, NULL };

        // Draw objects
        for (int j = 0; j < nodes.objects.size(); j++) {
            if (i == j) continue;

            R3SceneNode* ref_obj = nodes.objects[j];
            std::string ref_cat = GetObjectCategory(ref_obj, &id2cat);
            if (ref_cat.size() == 0) continue;
            
            switch ( task ) {
                case HEATMAPS:
                    CalcHeatmaps(pri_obj, ref_obj, ref_cat, pri_cat, &id2cat, &heatmaps, values, threshold, pixels_to_meters);
                    break;
                case INTRINSIC_PREPOSITIONS:
                    //CalcPreposisions();
                    break;
            }
        }

        // Draw walls
        for (int w = 0; w < nodes.walls.size(); w++) {
            R3SceneNode* wall_node = nodes.walls[w];
            
            switch ( task ) {
                case HEATMAPS:
                    CalcHeatmaps(pri_obj, wall_node, "wall", pri_cat, &id2cat, &heatmaps, values, threshold, pixels_to_meters);
                    break;
                case INTRINSIC_PREPOSITIONS:
                    //
                    break;
            }
        }
    }

    // Return success
    return 1;
}

int main(int argc, char **argv)
{
    // Parse program arguments
    if (!ParseArgs(argc, argv)) exit(-1);

    // Create a vector of all project IDs
    project_ids = LoadProjectIds();

    // Create mapping of ids to object categories
    id2cat = LoadIdToCategoryMap();

    // Create the output directory
    char cmd[1024];
    sprintf(cmd, "mkdir -p %s", output_grid_directory);
    system(cmd);

    int failures = 0;
    int i = 0; 
    for (std::string project_id : project_ids) // For each project...
    {
        if (i == 5) break;

        start = clock();
        fprintf(stdout, "Working on ... %s (%d) \n", project_id.c_str(), i); 

        // Read project
        P5DProject *project = ReadProject(project_id.c_str(), print_verbose, input_data_directory);
        if (!project) { failures++; continue; }
        fprintf(stdout, "\t- Read Project\n");

        // Create scene
        R3Scene *scene = CreateScene(project, input_data_directory);
        if (!scene) { failures++; continue; }
        fprintf(stdout, "\t- Created Scene\n");

        // Update the task goal
        if (!Update(scene)) { failures++; continue; }
        fprintf(stdout, "\t- Completed in : %f sec\n", (double)(clock() - start) / CLOCKS_PER_SEC);

        i++;
    }

    fprintf(stdout, "\n-- Failures: %d---\n", failures);
    fprintf(stdout, "\n--- Finished. ---\n");

    switch ( task ) {
        case HEATMAPS:
            WriteHeatmaps(&heatmaps, freq_stats, output_grid_directory, print_verbose);
            break;
        case INTRINSIC_PREPOSITIONS:
            // WriteIntrinsicPrepositions
            break;
    }

    // Return success 
    return 0;
}



