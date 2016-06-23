// This is the header file for the IO methods used in p5danalyze
// Moved for the sake of clarity

#ifndef IO_AUX
#define IO_AUX

#include "R3Graphics/R3Graphics.h"
#include "R3Graphics/p5d.h"
#include <string>
#include <vector>
#include <map>

using Id2CatMap = std::map<std::string, std::string>;

P5DProject *ReadProject(const char *project_name, bool print_verbose, const char *input_data_directory);
R3Scene *CreateScene(P5DProject *project, const char* input_data_directory);

std::vector<std::string> LoadProjectIds(std::string filename = "data/list-of-projects.txt");
Id2CatMap LoadIdToCategoryMap(std::string csv_filename="data/object_names.csv");
std::string GetObjectCategory(R3SceneNode* obj, Id2CatMap* id2cat);

int WriteGrid(R2Grid *grid, const char *filename, bool print_verbose);


#endif


