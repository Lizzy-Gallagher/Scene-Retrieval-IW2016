#include "R3Graphics/R3Graphics.h"
#include "R3Graphics/p5d.h"
#include "csv.h"

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>

using Id2CatMap = std::map<std::string, std::string>;

P5DProject *ReadProject(const char *project_name, bool print_verbose, const char *input_data_directory)
{
  // Start statistics
  RNTime start_time;
  start_time.Read();

  // Allocate scene
  P5DProject *project = new P5DProject();
  if (!project) {
    fprintf(stderr, "Unable to allocate project for %s\n", project_name);
    return NULL;
  }

  // Set project name
  project->name = strdup(project_name);

  // Read project from file
  char filename[4096];
  sprintf(filename, "%s/projects_room/%s/project.json", input_data_directory, project_name);
  if (!project->ReadFile(filename)) {
    delete project;
    return NULL;
  }

  // Print statistics
  if (print_verbose) {
    printf("Read project from %s ...\n", filename);
    printf("  Time = %.2f seconds\n", start_time.Elapsed());
    fflush(stdout);
  }

  // Return project
  return project;
}

R3Scene *
CreateScene(P5DProject *project, const char* input_data_directory) 
{
  // Start statistics
  RNTime start_time;
  start_time.Read();

  // Allocate scene
  R3Scene *scene = new R3Scene();
  if (!scene) {
    fprintf(stderr, "Unable to allocate scene\n");
    return NULL;
  }

  // Get node for project
  R3SceneNode *root_node = scene->Root();
  root_node->SetName("Project");

  // Create nodes for floors
  RNScalar floor_z = 0;
  for (int i = 0; i < project->NFloors(); i++) {
    P5DFloor *floor = project->Floor(i);

    // Create floor node
    char floor_name[1024];
    sprintf(floor_name, "Floor_%d", i);
    R3SceneNode *floor_node = new R3SceneNode(scene);
    floor_node->SetName(floor_name);
    floor_node->SetData(floor);
    floor->data = floor_node;
    root_node->InsertChild(floor_node);

    // Compute floor transformation
    R3Affine floor_transformation(R3identity_affine);
    floor_transformation.Translate(R3Vector(0, 0, floor_z));
    floor_z += floor->h;

    // Create nodes for rooms
    for (int j = 0; j < floor->NRooms(); j++) {
      P5DRoom *room = floor->Room(j);

      // Create room node
      char room_name[1024];
      sprintf(room_name, "Room_%d", j);
      R3SceneNode *room_node = new R3SceneNode(scene);
      room_node->SetName(room_name);
      room_node->SetData(room);
      room->data = room_node;
      floor_node->InsertChild(room_node);

      // Check if enclosed room
      if (!strcmp(room->className, "Room")) {
        // Read walls
        char rm_name[4096], node_name[4096];
        sprintf(rm_name, "%s/roomfiles/%s/fr_%drm_%d.obj", input_data_directory, project->name, i+1, room->idx_index+1); //fr_1rm_1.obj
        R3SceneNode *wall_node = new R3SceneNode(scene);
        sprintf(node_name, "Wall_%d", j);
        wall_node->SetName(node_name);

        RNArray<R3Material *> *materials = new RNArray<R3Material *>();
        if (ReadObj(scene, wall_node, rm_name, materials))
            room_node->InsertChild(wall_node);

        // Read floor
        sprintf(rm_name, "%s/roomfiles/%s/fr_%drm_%df.obj", input_data_directory, project->name, i+1, room->idx_index+1); //fr_1rm_1.obj
        R3SceneNode *rmfloor_node = new R3SceneNode(scene);
        sprintf(node_name, "Floor_%d", j);
        rmfloor_node->SetName(node_name);
        if (!ReadObj(scene, rmfloor_node, rm_name))
            room_node->InsertChild(rmfloor_node);
        
        // Read ceiling
        // sprintf(rm_name, "%s/roomfiles/%s/fr_%drm_%dc.obj", input_data_directory, project->name, i+1, room->idx_index+1); //fr_1rm_1.obj
        // R3SceneNode *rmceil_node = new R3SceneNode(scene);
        // sprintf(node_name, "Ceiling_%d", j);
        // rmceil_node->SetName(node_name);
        // if (!ReadObj(scene, rmceil_node, rm_name)) return 0;
        // room_node->InsertChild(rmceil_node);
      }
    }

    // Create nodes for objects
    for (int j = 0; j < floor->NObjects(); j++) {
      P5DObject *object = floor->Object(j);
      if (!object->id || !(*(object->id))) continue;

      // Create object node
      char object_name[1024];
      sprintf(object_name, "Object_%d_%s", j, object->id);
      R3SceneNode *object_node = new R3SceneNode(scene);
      object_node->SetName(object_name);
      object_node->SetData(object);
      object->data = object_node;

      // Insert into room or floor parent node
      R3SceneNode *parent_node = (object->room) ? floor_node->Child(object->room->floor_index) : floor_node;
      parent_node->InsertChild(object_node);

      // Compute object transformation
      R3Affine object_transformation(R3identity_affine);
      object_transformation.Translate(R3Vector(object->x, object->y, object->z));
      object_transformation.ZRotate(object->a);
      object_transformation.Scale(R3Vector(object->sX, object->sY, object->sZ));
      if (!strcmp(object->className, "Door")) object_transformation.ZRotate(RN_PI_OVER_TWO);
      else if (!strcmp(object->className, "Window")) object_transformation.ZRotate(RN_PI_OVER_TWO);
      if (object->fX) object_transformation.XMirror();
      if (object->fY) object_transformation.YMirror();

      // Read obj file
      char obj_name[4096];
      sprintf(obj_name, "%s/objects/%s/%s.obj", input_data_directory, object->id, object->id);
      if (!ReadObj(scene, object_node, obj_name)) return 0;

      // Apply transformation
      R3Affine transformation = R3identity_affine;
      transformation.Transform(floor_transformation);
      transformation.Transform(object_transformation);
      for (int k = 0; k < object_node->NElements(); k++) {
        R3SceneElement *element = object_node->Element(k);
        for (int s = 0; s < element->NShapes(); s++) {
          R3Shape *shape = element->Shape(s);
          shape->Transform(transformation);
        }
      }
    }
  }

  return scene;
}

std::vector<std::string>
LoadProjectIds(std::string filename = "data/list-of-projects.txt") {
    std::vector<std::string> project_ids;
    std::string id;
    std::ifstream project_ids_file;
    project_ids_file.open(filename);

    if (project_ids_file.is_open())
    {
        while (getline(project_ids_file, id))
        {
            project_ids.push_back(id);
        }
        project_ids_file.close();
    }

    return project_ids;
}

std::map<std::string, std::string> 
LoadIdToCategoryMap(std::string csv_filename="data/object_names.csv") { 
    std::map<std::string, std::string> id2cat;

    io::CSVReader<2, io::trim_chars<' '>, io::double_quote_escape<',','\"'> > in(csv_filename.c_str());
    in.read_header(io::ignore_extra_column, "id", "category");
    std::string id; std::string category;
    while(in.read_row(id, category)) {
        id2cat[id] = category;
    }

    return id2cat;
}

std::string GetObjectCategory(R3SceneNode* obj, Id2CatMap* id2cat) 
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

    // Lookup id in id2cat_map
    auto cat_iter = (*id2cat).find(id);
    if (cat_iter == (*id2cat).end()) {
        fprintf(stderr, "FAILURE. Unexpected object Id: %s\n", id.c_str());
        return "";
    }

    std::string cat = cat_iter->second;

    return cat;
}

void CreateDirectory(const char* dirname) {
    char cmd[1024];
    sprintf(cmd, "mkdir -p %s", dirname);
    system(cmd);
}

