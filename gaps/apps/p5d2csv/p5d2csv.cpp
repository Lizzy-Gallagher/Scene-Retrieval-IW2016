// Source file for the scene converter program



// Include files 

#include "R3Graphics/R3Graphics.h"
#include "p5d.h"
#include "unistd.h"



// Application Variables

static P5DProject *project = NULL;
static R3Scene *scene = NULL;



// Program Variables

static const char *input_project_name = NULL;
static const char *input_data_directory = "../..";
static int print_verbose = 0;
static int print_debug = 0;



static P5DProject *
ReadProject(const char *project_name)
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
  sprintf(filename, "%s/projects/%s/project.json", input_data_directory, project_name);
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

  // Temporary
  if (print_debug) project->Print();
  
  // Return project
  return project;
}


/*static R3Scene *CreateScene(P5DProject *project) 
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

    // Set floor transformation
    R3Affine floor_transformation(R3identity_affine);
    floor_transformation.Translate(R3Vector(0, 0, floor_z));
    floor_node->SetTransformation(floor_transformation);
    floor_z += floor->h;

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
      floor_node->InsertChild(object_node);

      // Set object transformation
      R3Affine object_transformation(R3identity_affine);
      object_transformation.Translate(R3Vector(object->x, object->y, object->z));
      object_transformation.ZRotate(object->a);
      object_transformation.Scale(R3Vector(object->sX, object->sY, object->sZ));
      if (!strcmp(object->className, "Door")) object_transformation.ZRotate(RN_PI_OVER_TWO);
      else if (!strcmp(object->className, "Window")) object_transformation.ZRotate(RN_PI_OVER_TWO);
      if (object->fX) object_transformation.XMirror();
      if (object->fY) object_transformation.YMirror();
      object_node->SetTransformation(object_transformation);

      // Read obj file
      char obj_name[4096];
      sprintf(obj_name, "%s/objects/%s/%s.obj", input_data_directory, object->id, object->id);
      if (!ReadObj(scene, object_node, obj_name)) return 0;
    }
    
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

      // Set room transformation
      // R3Affine room_transformation(R3identity_affine);
      // room_transformation.Translate(R3Vector(room->x, room->y, 0));
      // room_node->SetTransformation(room_transformation);

      // Create node for floor
      // ???
      
      // Check if enclosed room
      if (!strcmp(room->className, "Room")) {
        // Create nodes for ceiling
        // ???

#if 0        
        // Create nodes for walls
        for (int k = 0; k < room->NWalls(); k++) {
          P5DWall *wall = room->Wall(k);

          // Check if wall is hidden
          if (wall->hidden) continue;

          // Get vector for adding wall depth
          R3Point p1(wall->x1, wall->y1, 0);
          R3Point p2(wall->x2, wall->y2, 0);
          R3Vector d = p2 - p1;
          d.ZRotate(RN_PI_OVER_TWO);
          d.Normalize();
          d *= 0.5 * wall->w;

          // Get vector for adding wall height
          R3Vector h = room->h * R3posz_vector;

          // Get wall width
          RNScalar w = R3Distance(p1, p2);

          // Create wall vertices
          RNArray<R3TriangleVertex *> wall_vertices;
          wall_vertices.Insert(new R3TriangleVertex(p1 - d, R2Point(0,0)));
          wall_vertices.Insert(new R3TriangleVertex(p1 - d + h, R2Point(0,room->h)));
          wall_vertices.Insert(new R3TriangleVertex(p1 + d, R2Point(w,0)));
          wall_vertices.Insert(new R3TriangleVertex(p1 + d + h, R2Point(w,room->h)));
          wall_vertices.Insert(new R3TriangleVertex(p2 - d, R2Point(w,0)));
          wall_vertices.Insert(new R3TriangleVertex(p2 - d + h, R2Point(w,room->h)));
          wall_vertices.Insert(new R3TriangleVertex(p2 + d, R2Point(0,0)));
          wall_vertices.Insert(new R3TriangleVertex(p2 + d + h, R2Point(0,room->h)));

          // Create wall triangles
          RNArray<R3Triangle *> wall_triangles;
          wall_triangles.Insert(new R3Triangle(wall_vertices[0], wall_vertices[4], wall_vertices[5]));
          wall_triangles.Insert(new R3Triangle(wall_vertices[0], wall_vertices[5], wall_vertices[1]));
          wall_triangles.Insert(new R3Triangle(wall_vertices[2], wall_vertices[7], wall_vertices[6]));
          wall_triangles.Insert(new R3Triangle(wall_vertices[2], wall_vertices[3], wall_vertices[7]));
          R3Shape *wall_shape = new R3TriangleArray(wall_vertices, wall_triangles);

          // Create wall material

          // Create wall element
          R3SceneElement *wall_element = new R3SceneElement();
          wall_element->SetMaterial(&R3default_material);
          wall_element->InsertShape(wall_shape);         
        
          // Create wall node
          char wall_name[1024];
          sprintf(wall_name, "Wall_%d", k);
          R3SceneNode *wall_node = new R3SceneNode(scene);
          wall_node->InsertElement(wall_element);
          wall_node->SetName(wall_name);
          wall_node->SetData(wall);
          wall->data = wall_node;
          room_node->InsertChild(wall_node);
        }
#else
        // Read walls
        char rm_name[4096], node_name[4096];
        sprintf(rm_name, "%s/roomfiles/%s/fr_%drm_%d.obj", input_data_directory, project->name, i+1, room->idx_index+1); //fr_1rm_1.obj
        R3SceneNode *wall_node = new R3SceneNode(scene);
        sprintf(node_name, "WALL_%d", room->idx_index);
        wall_node->SetName(node_name);
        if (!ReadObj(scene, wall_node, rm_name)) return 0;
        room_node->InsertChild(wall_node);

        // Read floor
        sprintf(rm_name, "%s/roomfiles/%s/fr_%drm_%df.obj", input_data_directory, project->name, i+1, room->idx_index+1); //fr_1rm_1.obj
        R3SceneNode *rmfloor_node = new R3SceneNode(scene);
        sprintf(node_name, "FLOOR_%d", room->idx_index);
        rmfloor_node->SetName(node_name);
        if (!ReadObj(scene, rmfloor_node, rm_name)) return 0;
        room_node->InsertChild(rmfloor_node);
        
        // Read ceiling
        // sprintf(rm_name, "%s/roomfiles/%s/fr_%drm_%dc.obj", input_data_directory, project->name, i+1, room->idx_index+1); //fr_1rm_1.obj
        // R3SceneNode *rmceil_node = new R3SceneNode(scene);
        // sprintf(node_name, "CEILING_%d", room->idx_index);
        // rmceil_node->SetName(node_name);
        // if (!ReadObj(scene, rmceil_node, rm_name)) return 0;
        // room_node->InsertChild(rmceil_node);
#endif
      }
    }
  }

  // Print statistics
  if (print_verbose) {
    printf("Created scene ...\n");
    printf("  Time = %.2f seconds\n", start_time.Elapsed());
    printf("  # Nodes = %d\n", scene->NNodes());
    fflush(stdout);
  }

  // Return scene
  return scene;
}*/

////////////////////////////////////////////////////////////////////////
// MAIN LOOP
////////////////////////////////////////////////////////////////////////

    static int
ParseProject(P5DProject *project, FILE* scenesFile, FILE* floorsFile, FILE* roomsFile, FILE* objsFile )
{
    fprintf(scenesFile, "p5d_id,nfloors,nrooms,nobjs\n");
    fprintf(floorsFile, "scene_id,nobjs,nrooms,area,floor\n");
    fprintf(roomsFile, "floor_id,type_id,area,p5_id\n");
    fprintf(objsFile, "model_id,room_id,x,y,z,sX,sY,sZ,a,fX,fY\n");
    
    char* name = project->name;
    int nFloors = project->NFloors();
    int nTotalObjs = 0;
    int nTotalRooms = 0;
    for (int i = 0; i < nFloors; i++) {
        P5DFloor *floor = project->Floor(i);
        
        int nRooms = floor->NRooms();
        nTotalRooms += nRooms;
        /*
        for (int r = 0; r < nRooms; r++) {
            // Work with rooms
        }
        */

        int nObjects = floor->NObjects();
        nTotalObjs += nObjects;
        for (int o = 0; o < nObjects; o++) {
            // Work with objects
            P5DObject *obj = floor->Object(o);
            char* obj_id = obj->id;
            double x = obj->x;
            double y = obj->y;
            double z = obj->z;
            double sX = obj->sX;
            double sY = obj->sY;
            double sZ = obj->sZ;
            double a = obj->a;
            int fX = obj->fX;
            int fY = obj->fY;

            // TODO
            char* room_id = "";
            char* model_id = "";

            fprintf(objsFile, "%s,%s,%f,%f,%f,%f,%f,%f,%f,%d,%d\n",
                    model_id,room_id,x,y,z,sX,sY,sZ,a,fX,fY);
        }

        // TODO
        char* scene_id = "";
        double area = -1.0;

        fprintf(floorsFile, "%s,%d,%d,%f,%d\n",
                scene_id,nObjects,nRooms,area,i);
    }

    fprintf(scenesFile, "%s,%d,%d,%d\n", 
            name, nFloors, nTotalRooms, nTotalObjs);

    return 0;
}


////////////////////////////////////////////////////////////////////////
// PROGRAM ARGUMENT PARSING
////////////////////////////////////////////////////////////////////////

    static int
ParseArgs(int argc, char **argv)
{
    // Parse arguments
    argc--; argv++;
    while (argc > 0) {
        if ((*argv)[0] == '-') {
            if (!strcmp(*argv, "-v")) print_verbose = 1;
            else { fprintf(stderr, "Invalid program argument: %s", *argv); exit(1); }
            argv++; argc--;
        }
        else {
            if (!input_project_name) input_project_name = *argv;
            else { fprintf(stderr, "Invalid program argument: %s", *argv); exit(1); }
            argv++; argc--;
        }
    }

    // Check input filename
    if (!input_project_name) {
        fprintf(stderr, "Usage: scn2csv input_project_name [options]\n");
        return 0;
    }

    // Return OK status 
    return 1;
}



////////////////////////////////////////////////////////////////////////
// MAIN
////////////////////////////////////////////////////////////////////////


int main(int argc, char **argv)
{
    // Check number of arguments
    if (!ParseArgs(argc, argv)) exit(1);
    
    // Rename to actual projectname.csv 
    // because not loading everything, but individually
    const char* scenesFile = "scenes.csv";
    const char* floorsFile = "floors.csv";
    const char* roomsFile  = "rooms.csv";
    const char* objsFile   = "objs.csv";

    // Open scenes file
    FILE *scenesFp = fopen(scenesFile, "w");
    if (!scenesFp) {
        fprintf(stderr, "Unable to open output file %s\n", floorsFile);
        return 0;
    }
    FILE *floorsFp = fopen(floorsFile, "w");
    if (!floorsFp) {
        fprintf(stderr, "Unable to open output file %s\n", floorsFile);
        return 0;
    }
    FILE *roomsFp = fopen(roomsFile, "w");
    if (!roomsFp) {
        fprintf(stderr, "Unable to open output file %s\n", roomsFile);
        return 0;
    }
    FILE *objsFp = fopen(objsFile, "w");
    if (!objsFp) {
        fprintf(stderr, "Unable to open output file %s\n", objsFile);
        return 0;
    }


    // Open rooms file
    // Open objects file
    

    // Read Project
    project = ReadProject(input_project_name);
    if (!project) exit(-1);

    fprintf(stdout, "NFloors: %d\n", project->NFloors());

    ParseProject(project, scenesFp, floorsFp, roomsFp, objsFp);

    // Create scene
    //scene = CreateScene(project);
    //if (!scene) exit(-1);

    // Return success 
    return 0;
}
