// Source file for the scene analyzer program

// Include files 

#include "R3Graphics/R3Graphics.h"
#include "fglut/fglut.h"
#include "R3Graphics/p5d.h"
#include "csv.h"
#include "R2Aux.h"
#include "IOAux.h"

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <time.h>

// Program variables
static const char *input_data_directory = "data/";
static const char *output_grid_directory = NULL;
static int print_verbose = 0;
static int print_debug = 0;

// Logging
static std::map<std::string, int> num_of_object;
static std::map<std::string, std::map<std::string, int>> num_of_pair;

static std::map<std::string, std::string> id_to_category;
static std::map<std::string, std::map<std::string, R2Grid*>> grids;
static std::vector<std::string> project_ids;

static int meters_of_context = 3;
static int pixels_to_meters = 15;
static int resolution = pixels_to_meters * (2 * meters_of_context);

static double threshold = meters_of_context + 1;
clock_t start, finish;

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
  if (!output_grid_directory) {
    fprintf(stderr, "Usage: p5dview outputgriddirectory [-debug] [-v]" 
            "[data-directory dir] [-ptm num] [-context num] [-category id]  \n");
    return 0;
  }

  // Return OK status 
  return 1;
}

std::string GetObjectCategory(R3SceneNode* obj) 
{
    // Parse object name for id
    std::string name (obj->Name());

    size_t pos = name.find_last_of("_") + 1;
    if (name[pos - 2] == '_') { // for the s__* series
        pos = name.find_last_of("_", pos - 3) + 1;
    }

    if (pos == std::string::npos) {
        fprintf(stdout, "FAILURE. Malformed object name: %s\n", name.c_str());
        exit(-1);
    }
    
    std::string id = name.substr(pos);

    // Lookup id in id_to_category_map
    auto cat_iter = id_to_category.find(id);
    if (cat_iter == id_to_category.end()) {
        fprintf(stderr, "FAILURE. Unexpected object Id: %s\n", id.c_str());
        return "";
    }

    std::string cat = cat_iter->second;
    
    if (print_verbose) {
        fprintf(stdout, "Id: %s Category: %s\n", id.c_str(), cat.c_str());
    }

    return cat;
}

/*-------------------------------------------------------------------------*/
//  Heatmap Code
/*-------------------------------------------------------------------------*/


static R2Grid*
DrawObject(R3SceneNode* obj, R2Grid *grid, R2Vector translation, RNAngle theta, bool do_fX, bool do_fY, R2Vector *dist = NULL)
{
    R2Grid temp_grid = R2Grid(resolution, resolution);
    
    R2Affine world_to_grid_xform = PrepareWorldToGridXform(obj->Centroid(), translation, theta, do_fX, do_fY, dist, pixels_to_meters);
    temp_grid.SetWorldToGridTransformation(world_to_grid_xform);
    
    // For all R3SceneElements in the R3SceneNode
    for (int k = 0; k < obj->NElements(); k++) {
        R3SceneElement* el = obj->Element(k);
        
        // For all R3Shapes in the R3SceneElements    
        for (int l = 0; l < el->NShapes(); l++) {

            R3Shape* shape = el->Shape(l);
            R3TriangleArray* arr = (R3TriangleArray*) shape;
            
            // For all R3Triangles in the R3TriangleArray
            for (int t = 0; t < arr->NTriangles(); t++) {
                R3Triangle *triangle = arr->Triangle(t);

                // Create new points
                R2Point v0 = R2Point(triangle->V0()->Position().X(), triangle->V0()->Position().Y());
                R2Point v1 = R2Point(triangle->V1()->Position().X(), triangle->V1()->Position().Y());
                R2Point v2 = R2Point(triangle->V2()->Position().X(), triangle->V2()->Position().Y());

                if (IsOutsideGrid(&temp_grid, v0, v1, v2)) continue;

                // Move exterior verticies inside the grid
                std::vector<R2Point> v = MoveInsideGrid(&temp_grid, v0, v1, v2);
                
                v0 = temp_grid.GridPosition(v[0]);
                v1 = temp_grid.GridPosition(v[1]);
                v2 = temp_grid.GridPosition(v[2]);

                temp_grid.RasterizeWorldTriangle(v[0], v[1], v[2], 1);
            }
        }
    }

    // Color the shape with a single color
    temp_grid.Threshold(0, 0, 1);
    grid->Add(temp_grid);
    return grid;
}

// Take collection of grids and write them
static int 
WriteHeatMaps()
{
    std::ofstream stats_categories_file;
    stats_categories_file.open("stats/categories.csv");
    stats_categories_file << "category,count\n"; 

    std::ofstream stats_pairs_file;
    stats_pairs_file.open("stats/pairs.csv");
    stats_pairs_file << "src_cat,dst_cat,count\n"; 

  for (auto it : grids) {
    std::string src_cat = it.first;
    std::map<std::string, R2Grid*> map = it.second;
    stats_categories_file << src_cat.c_str() << "," << num_of_object[src_cat] << "\n";


    for (auto it2 : map) {
        std::string dst_cat = it2.first;
        R2Grid* grid        = it2.second;

        char output_filename[1024];
        sprintf(output_filename, "%s/%s___%s.grd", output_grid_directory, 
                src_cat.c_str(), dst_cat.c_str());

        stats_pairs_file << src_cat.c_str() << "," << dst_cat.c_str() << "," << num_of_pair[src_cat][dst_cat] << "\n"; 
        if (!WriteGrid(grid, output_filename, print_verbose)) return 0;
    }
  }

  stats_categories_file.close();
  stats_pairs_file.close();
  return 1;
}

static int
UpdateHeatmapCollection(R3Scene* scene, std::vector<R3SceneNode*> object_nodes)
{
  
  // Populate category->category map
  for (int i = 0; i < object_nodes.size(); i++) {
    R3SceneNode* src_obj = object_nodes[i];
    std::string src_cat = GetObjectCategory(src_obj);
    if (src_cat.size() == 0) continue; 
    //if (src_cat != only_category) continue;

    num_of_object[src_cat]++;

    for (int j = 0; j < object_nodes.size(); j++) {
        if (i == j) continue;
        
        R3SceneNode* dst_obj = object_nodes[j];
        std::string dst_cat = GetObjectCategory(dst_obj);
        if (dst_cat.size() == 0) continue;

        if (grids[src_cat].count(dst_cat) == 0)
            grids[src_cat][dst_cat] = new R2Grid(resolution, resolution); 
    }
    
    if (grids[src_cat]["wall"] == 0)
        grids[src_cat]["wall"] = new R2Grid(resolution, resolution);
  }
 
  return 1;
}

static int
UpdateGrids(R3Scene *scene)
{
  std::vector<R3SceneNode*> object_nodes;
  std::vector<R3SceneNode*> wall_nodes;

  // Find all objects and walls
  for (int i = 0; i < scene->NNodes(); i++) {
    R3SceneNode* node = scene->Node(i);

    std::string name (node->Name());
    if (std::string::npos != name.find("Object")) // probably a better way with casting
        object_nodes.push_back(node);
    if (std::string::npos != name.find("Wall"))
        wall_nodes.push_back(node);     
  }
    
    fprintf(stdout, "\t- Located Objects : %lu\n", object_nodes.size());

  // Category->Category->Heatmap
  if (!UpdateHeatmapCollection(scene, object_nodes)) return 0;
        
  fprintf(stdout, "\t- Updated Heatmaps\n");

  for (int i = 0; i < object_nodes.size(); i++) {
    R3SceneNode* src_obj = object_nodes[i];
    std::string src_cat = GetObjectCategory(src_obj);
    if (src_cat.size() == 0) continue;
    //if (src_cat != only_category) continue;

    // Translation constants
    float a = 0.5 * (resolution - 1) - src_obj->Centroid().X();
    float b = 0.5 * (resolution - 1) - src_obj->Centroid().Y();
    R2Vector translation(a, b);
    
    // Rotation constants
    P5DObject *p5d_obj = (P5DObject *) src_obj->Data();
    RNAngle theta = p5d_obj->a;
    
    if (!strcmp(p5d_obj->className, "Door")) theta += RN_PI_OVER_TWO;
    else if (!strcmp(p5d_obj->className, "Window")) theta += RN_PI_OVER_TWO;
    
    bool do_fX = p5d_obj->fX;
    bool do_fY = p5d_obj->fY;

    // Draw objects
    for (int j = 0; j < object_nodes.size(); j++) {
        if (i == j) continue;
        
        R3SceneNode* dst_obj = object_nodes[j];
        std::string dst_cat = GetObjectCategory(dst_obj);
        if (dst_cat.size() == 0) continue;

        R2Grid *grid = grids[src_cat][dst_cat];
        
        R3Vector dist3d = (dst_obj->Centroid() - src_obj->Centroid());
        R2Vector dist = R2Vector(dist3d.X(), dist3d.Y());

        // Only draw object close enough
        if (dist.Length() < threshold) {
            DrawObject(dst_obj, grid, translation, theta, do_fX, do_fY, &dist);
            num_of_pair[src_cat][dst_cat]++;
        }
    }

    // Draw walls
    for (int w = 0; w < wall_nodes.size(); w++) {
        R3SceneNode* wall_node = wall_nodes[w];

        R2Grid *grid = grids[src_cat]["wall"];


        R3Vector dist3d = (wall_node->Centroid() - src_obj->Centroid());
        R2Vector dist = R2Vector(dist3d.X(), dist3d.Y());
        
        if (dist.Length() < threshold) {
            DrawObject(wall_node, grid, translation, theta, do_fX, do_fY, &dist);
            num_of_pair[src_cat]["wall"]++;
        }
    }

  }

  // Return success
  return 1;
}

int main(int argc, char **argv)
{
    // Create a vector of all project IDs
    project_ids = LoadProjectIds();
    
    // Create mapping of ids to object categories
    id_to_category = LoadIdToCategoryMap()) exit(-1);

    // Parse program arguments
    if (!ParseArgs(argc, argv)) exit(-1);

    // Create the output directory
    char cmd[1024];
    sprintf(cmd, "mkdir -p %s", output_grid_directory);
    system(cmd);

    int failures = 0;
    int i = 0; 
    for (std::string project_id : project_ids)
    {
        if (i == 50) break;
        
        start = clock();
        fprintf(stdout, "Working on ... %s (%d) \n", project_id.c_str(), i); 

        // Read project
        P5DProject *project = ReadProject(project_id.c_str(), print_verbose);
        if (!project) /*exit(-1)*/ {
            failures++;
            continue;
        }

        fprintf(stdout, "\t- Read Project\n");

        // Create scene
        R3Scene *scene = CreateScene(project);
        if (!scene) {
            failures++;
            continue;
        }

        fprintf(stdout, "\t- Created Scene\n");

        // Write grids
        if (!UpdateGrids(scene)) {
            failures++;
            continue;
        }

        fprintf(stdout, "\t- Completed in : %f sec\n", (double)(clock() - start) / CLOCKS_PER_SEC);
        i++;
    }

    fprintf(stdout, "\n-- Failures: %d---\n", failures);
    fprintf(stdout, "\n--- Finished. ---\n");

    WriteHeatMaps();

    // Return success 
    return 0;
}



