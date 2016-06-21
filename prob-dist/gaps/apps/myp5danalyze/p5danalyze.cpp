// Source file for the scene viewer program

// Old

// Include files 

#include "R3Graphics/R3Graphics.h"
#include "fglut/fglut.h"
#include "p5d.h"

#include <string>
#include <map>
#include <math.h>

// Program variables

static const char *input_project_name = NULL;

//static const char *input_object_directory = "../../objects";
//static const char *input_texture_directory = "../../texture";

static const char *input_object_directory = "data/objects";
static const char *input_texture_directory = "data/texture";

static const char *output_grid_directory = NULL;
static int print_verbose = 0;
static int print_debug = 0;



static P5DProject *
ReadProject(const char *filename)
{
  // Start statistics
  RNTime start_time;
  start_time.Read();

  // Allocate scene
  P5DProject *project = new P5DProject();
  if (!project) {
    fprintf(stderr, "Unable to allocate project for %s\n", filename);
    return NULL;
  }

  // Read project from file
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



static int 
WriteGrid(R2Grid *grid, const char *filename)
{
  // Start statistics
  RNTime start_time;
  start_time.Read();

  // Write grid
  int status = grid->Write(filename);

  // Print statistics
  if (print_verbose) {
    printf("Wrote grid to %s\n", filename);
    printf("  Time = %.2f seconds\n", start_time.Elapsed());
    printf("  Resolution = %d %d\n", grid->XResolution(), grid->YResolution());
    printf("  Spacing = %g\n", grid->GridToWorldScaleFactor());
    printf("  Cardinality = %d\n", grid->Cardinality());
    RNInterval grid_range = grid->Range();
    printf("  Minimum = %g\n", grid_range.Min());
    printf("  Maximum = %g\n", grid_range.Max());
    printf("  L1Norm = %g\n", grid->L1Norm());
    printf("  L2Norm = %g\n", grid->L2Norm());
    fflush(stdout);
  }

  // Return status
  return status;
}



static R3Scene *
CreateScene(P5DProject *project)
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
      sprintf(obj_name, "%s/%s/%s.obj", input_object_directory, object->id, object->id);
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
      R3Affine room_transformation(R3identity_affine);
      room_transformation.Translate(R3Vector(room->x, room->y, 0));
      // room_transformation.Scale(R3Vector(room->sX, room->sY, 1.0));
      room_node->SetTransformation(room_transformation);

      // Create node for floor
      // ???
      
      // Check if enclosed room
      if (!strcmp(room->className, "Room")) {
        // Create nodes for ceiling
        // ???
      
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
}



static int
WriteGrids(R3Scene *scene)
{
  // Create the output directory
  char cmd[1024];
  sprintf(cmd, "mkdir -p %s", output_grid_directory);
  system(cmd);

  // Create a grid
  R2Grid grid(32,32);

  // Fill in the grid
  grid.RasterizeGridPoint(16 , 16, 1.0);
  grid.Blur(2);
  
  // Write the grid
  char output_filename[1024];
  sprintf(output_filename, "%s/foo.grd", output_grid_directory);
  if (!WriteGrid(&grid, output_filename)) return 0;

  // Return success
  return 1;
}


static int 
ParseArgs(int argc, char **argv)
{
  // Parse arguments
  argc--; argv++;
  while (argc > 0) {
    if ((*argv)[0] == '-') {
      if (!strcmp(*argv, "-v")) print_verbose = 1; 
      else if (!strcmp(*argv, "-debug")) print_debug = 1; 
      else if (!strcmp(*argv, "-object_directory")) { argc--; argv++; input_object_directory = *argv; }
      else if (!strcmp(*argv, "-texture_directory")) { argc--; argv++; input_texture_directory = *argv; }
      else { 
        fprintf(stderr, "Invalid program argument: %s", *argv); 
        exit(1); 
      }
      argv++; argc--;
    }
    else {
      if (!input_project_name) input_project_name = *argv;
      else if (!output_grid_directory) output_grid_directory = *argv;
      else { fprintf(stderr, "Invalid program argument: %s", *argv); exit(1); }
      argv++; argc--;
    }
  }

  // Check filenames
  if (!input_project_name || !output_grid_directory) {
    fprintf(stderr, "Usage: p5dview inputprojectfile outputgriddirectory\n");
    return 0;
  }

  // Return OK status 
  return 1;
}

int parse_id(R3SceneNode* node) {
  std::string name (node->Name());

  size_t pos = name.find_last_of("_") + 1;
  std::string id = name.substr(pos);

  return std::stoi(id);
} 

static int
MyWriteGrids(R2Grid grid, int name)
{
  // Create the output directory
  char cmd[1024];
  sprintf(cmd, "mkdir -p %s", output_grid_directory);
  system(cmd);

  // Create a grid
  //R2Grid grid(32,32);

  // Fill in the grid
  //grid.RasterizeGridPoint(16 , 16, 1.0);
  //grid.Blur(2);
  
  // Write the grid
  char output_filename[1024];
  sprintf(output_filename, "%s/%s.grd", output_grid_directory, std::to_string(name).c_str());
  if (!WriteGrid(&grid, output_filename)) return 0;

  // Return success
  return 1;
}


int main(int argc, char **argv)
{
  // Parse program arguments
  if (!ParseArgs(argc, argv)) exit(-1);

  // Read project
  P5DProject *project = ReadProject(input_project_name);
  if (!project) exit(-1);

  // Create scene
  R3Scene *scene = CreateScene(project);
  if (!scene) exit(-1);

  // Testing...
  fprintf(stdout, "Testing, should be a number: %d\n", scene->NNodes());
  fprintf(stdout, "Testing, should be a name: %s\n", scene->Node(1)->Name());
  fprintf(stdout, "Nodes:\n"); 
  
  // Collect all Object nodes
  std::vector<R3SceneNode*> object_nodes;
  std::vector<R3SceneNode*> wall_nodes; // potentia
  std::vector<R3SceneNode*> floor_nodes;
  for (int i = 0; i < scene->NNodes(); i++) {
      R3SceneNode *node = scene->Node(i);

      // If not an object (e.g. wall, floor, room), skip
      std::string name (node->Name());
      if (std::string::npos == name.find("Object")) continue;

      object_nodes.push_back(node);

      //int id = parse_id(node);
      fprintf(stdout, "%s id:%d x:%f y:%f\n", node->Name(), 0,  node->Centroid().X(), node->Centroid().Y());
  }

  // For all nodes
  for (int i = 0; i < scene->NNodes(); i++) {
    R3SceneNode* node = scene->Node(i); 
    fprintf(stdout, "Starting node %d\n", i);

    // For all elements
    for (int j = 0; j < node->NElements(); j++) {
        R3SceneElement* el = node->Element(j);
        fprintf(stdout, "\tStarting element %d\n", j);

        // For all shapes
        for (int k = 0; k < el->NShapes(); k++) {
            R3Shape* shape = el->Shape(k);
            fprintf(stdout, "\t\tStarting shape %d\n", k);

            fprintf(stdout, "\t\t\tArea: %f\n", shape->Area());
            fprintf(stdout, "\t\t\tVolume: %f\n", shape->Volume());

            // Type?
            fprintf(stdout, "\t\t\tIsPoint: %d\n", shape->IsPoint());
            fprintf(stdout, "\t\t\tIsCurve: %d\n", shape->IsCurve());
            fprintf(stdout, "\t\t\tIsLinear: %d\n", shape->IsLinear());
            fprintf(stdout, "\t\t\tIsSurface: %d\n", shape->IsSurface());
            fprintf(stdout, "\t\t\tIsPlanar: %d\n", shape->IsPlanar());
            fprintf(stdout, "\t\t\tIsSolid: %d\n", shape->IsSolid());
            fprintf(stdout, "\t\t\tIsConvex: %d\n", shape->IsConvex());

            // Mini-Box
            R3Box test_box = R3Box(0, 0, 0, 1, 1, 1);
            R3Box test_box2 = R3Box(0, 0, 0, 2, 2, 2); 

            bool does_intersect = R3Intersects(test_box, test_box2);
            fprintf(stdout, "\t\t\tdoes_intersect: %d\n", does_intersect);
        }
    }
  }

  /*std::map<int, std::map<int, int> > temp; // intermediate (id->id->count)
  for (int i = 0; i < object_nodes.size(); i++) {
    int id = parse_id(object_nodes[i]);
    temp[id] = std::map<int,int>();
  }*/

  // For each unique object id, create a grid
  /*std::map<int, R2Grid> id_to_grid;
  for (int i = 0; i < object_nodes.size(); i++) {
    R2Grid grid(32,32);
    grid.RasterizeGridPoint(16 , 16, 1.0); 
    
    int id = parse_id(object_nodes[i]); 
    id_to_grid[id] = grid;
  }*/


  // Pair-wise comparison
  for (int i = 0; i < object_nodes.size(); i++) {
    for (int j = 0; j < object_nodes.size(); j++) {
        if (i == j) continue;

        R3SceneNode* src_node = object_nodes[i];
        R3SceneNode* tgt_node = object_nodes[j];

        R2Grid grid(32,32);
        grid.RasterizeGridPoint(16,16, 1.0); // src
        
        int x = round((tgt_node->Centroid().X() - src_node->Centroid().X())*3) + 16;
        int y = round((tgt_node->Centroid().Y() - src_node->Centroid().Y())*3) + 16;

        //fprintf(stdout, "x: %d, y: %d\n", x, y);

        grid.RasterizeGridPoint(x, y, 1.0); // tgt

        MyWriteGrids(grid, i * 100 + j);
        //int src_id = parse_id(src_node);
        //int tgt_id = parse_id(tgt_node);

        // Update relationships in temp
        //temp[src_id][tgt_id] = temp[src_id][tgt_id] + 1;

        //fprintf(stdout, "%d : %d\n", src_id, tgt_id);
    }
  }

  // print result of temp
  /*for (auto it : temp) {
     for (auto it2 : it.second) {
       fprintf(stdout, "src: %d tgt: %d cnt: %d\n", it.first, it2.first, it2.second);
     }
  }*/


  // Write grids
  if (!WriteGrids(scene)) exit(-1);
  
  // Return success 
  return 0;
}

