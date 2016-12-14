// Source file for the heatmap collation project

// Include files 
#include "R3Graphics/R3Graphics.h"
#include "fglut/fglut.h"

#include <iostream>
#include <fstream>
#include <vector>

// Program variables
static const char *heatmaps_filename;
clock_t start, finish;

static int ParseArgs(int argc, char **argv)
{
    // Parse arguments
    argc--; argv++;
    while (argc > 0) {
        if (!heatmaps_filename) heatmaps_filename = *argv;
        else { fprintf(stderr, "Invalid program argument: %s", *argv); exit(1); }
        argv++; argc--;
    }

    // Check filenames
    if (!heatmaps_filename) {
        fprintf(stderr, "Usage: htm2htm filename\n");
        return 0;
    }

    // Return OK status 
    return 1;
}

std::vector<std::string>
LoadHeatmaps() {
    std::vector<std::string> heatmap_names;
    std::string name;
    std::ifstream heatmaps_file;
    heatmaps_file.open(heatmaps_filename);

    if (heatmaps_file.is_open())
    {
        while (getline(heatmaps_file, name)) { heatmap_names.push_back(name); }
        heatmaps_file.close();
    }

    return heatmap_names;
}



int main(int argc, char **argv)
{
    // Parse program arguments
    if (!ParseArgs(argc, argv)) exit(1);

    std::vector<std::string> heatmap_names = LoadHeatmaps();

    R2Grid* collation = new R2Grid();
    collation->ReadImage(heatmap_names[0].c_str());
    for (int i = 1; i < heatmap_names.size(); i++) {
        fprintf(stderr, "Working on %d\n", i);
        R2Grid* next = new R2Grid();
        fprintf(stderr, "About to read...\n");
        next->ReadImage(heatmap_names[i].c_str());
        fprintf(stderr, "Finished reading...\n");
        *collation += *next;
    }

    collation->WriteFile("test.jpg");

    // Return success 
    return 0;
}




