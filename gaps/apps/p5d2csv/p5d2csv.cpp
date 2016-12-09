//Source file for the scene converter program


// Include files 

#include "R3Graphics/R3Graphics.h"
#include "p5d.h"
#include "unistd.h"
#include <vector>
#include <string>
#include <string.h>
#include <fstream>

// Application Variables

static std::vector<std::string> project_ids;
static std::vector<std::string> model_ids;
static std::vector<std::string> rtype_ids;

// Program Variables
static const char *output_directory = NULL;

static const char *project_list = "../../../data/list-of-projects.txt";
static const char *rtype_list = "../../../data/list-of-rtypes.txt";

static int print_verbose = 0;

////////////////////////////////////////////////////////////////////////
// Load Data
////////////////////////////////////////////////////////////////////////

static int LoadRTypes() {
    std::ifstream file(rtype_list);
    std::string str;
    while (std::getline(file, str))
    {
        rtype_ids.push_back(str);
    }

    return 1;
}

static int LoadProjects() {
    std::ifstream file(project_list);
    std::string str;
    while (std::getline(file, str))
    {
        project_ids.push_back(str);
    }

    return 1;
}

static int 
getRTypeId(const char* name)
{
    if (!name)
        return -1;

    for (size_t i = 0; i < rtype_ids.size(); i++) {
        if (strcmp(rtype_ids[i].c_str(), name) == 0) return i;
    }

    return -1;
}

static int 
getP5DId(const char* name)
{
    for (size_t i = 0; i < project_ids.size(); i++) {
        if (strcmp(project_ids[i].c_str(), name) == 0) return i;
    }

    return -1;
}

static const char*
getModelId(const char* object_name) {
    const char* door_id = strstr(object_name, "Door");
    if (door_id)
        return strlen("Door_") + door_id;

    const char* window_id = strstr(object_name, "Window");
    if (window_id)
        return strlen("Window_") + window_id;

    const char* model_id = strstr(object_name, "Ns_") + strlen("Ns_");
    return model_id;
}

////////////////////////////////////////////////////////////////////////
// AUX
////////////////////////////////////////////////////////////////////////

int outdoor_object_num = 0;
int PrintOutdoorObject(FILE* objs_file, R3SceneNode* obj_node, int p5d_id, int floor_num, int room_num = -1){
        int object_num = outdoor_object_num++;

        P5DObject *obj = (P5DObject*) obj_node->Data();
        if (!obj) // Floors, Walls, etc.
            return 0;

        double x = obj->x;
        double y = obj->y;
        double z = obj->z;
        double sX = obj->sX;
        double sY = obj->sY;
        double sZ = obj->sZ;
        double a = obj->a;
        int fX = obj->fX;
        int fY = obj->fY;
        int floor_index = obj->floor_index;

        const char* model_id = getModelId(obj_node->Name());

        fprintf(objs_file, "%d,%d,%d,%d,%s,%f,%f,%f,%f,%f,%f,%f,%d,%d,%d\n",
                p5d_id,floor_num+1,room_num+1,object_num+1,model_id,
                x,y,z,sX,sY,sZ,a,fX,fY,floor_index);
        
        return 1;
}




////////////////////////////////////////////////////////////////////////
// MAIN LOOP
////////////////////////////////////////////////////////////////////////

static int
ParseScene(R3Scene *scene, FILE* scenes_file, FILE* floors_file, FILE* rooms_file, FILE* objs_file, FILE* rtypes_file )
{
    fprintf(scenes_file, "id,num_floors,num_rooms,num_objects,hash\n");
    fprintf(floors_file, "scene_id,floor_num,num_rooms,num_objects,area\n");
    fprintf(rooms_file, "scene_id,floor_num,room_num,num_objects,area,floor_index\n");
    fprintf(objs_file, "scene_id,floor_num,room_num,object_num,model_id,x,y,z,sX,sY,sZ,a,fX,fY,floor_index\n");
    fprintf(rtypes_file, "scene_id,floor_num,room_num,floor_index,room_type_id\n");

    R3SceneNode* root = scene->Node(0);

    const char* name = root->Name();
    size_t prefix_len = strlen("Project#");
    name = name + prefix_len;

    int p5d_id = getP5DId(name);

    int nFloors = root->NChildren();
    int nTotalRooms = 0;
    int nTotalObjs = 0;

    // Floors
    for (int floor_num = 0; floor_num < root->NChildren(); floor_num++) {
        R3SceneNode* floor = root->Child(floor_num);

        int nObjsFloor = 0;
        int nRooms = 0;
        
        // Rooms
        for (int room_num = 0; room_num < floor->NChildren(); room_num++) {
            R3SceneNode* node = floor->Child(room_num);
            
            if (strstr(node->Name(), "Object")) {
                nObjsFloor++;
                PrintOutdoorObject(objs_file, node, p5d_id, floor_num);
                continue;
            }
 
            else if (strstr(node->Name(), "Room")) {
                P5DRoom* room = (P5DRoom*) node->Data();
                if (!strstr(room->className, "Room")) // Ignore "Ground"
                    continue;

                nRooms++;
                int nObjectsRoom = 0;

                // Objects
                for (int object_num = 0; object_num < node->NChildren(); object_num++) {
                    R3SceneNode* obj_node = node->Child(object_num);
                    P5DObject *obj = (P5DObject*) obj_node->Data();

                    if (!obj) // Floors, Walls, etc.
                        continue;

                    nObjectsRoom++;

                    double x = obj->x;
                    double y = obj->y;
                    double z = obj->z;
                    double sX = obj->sX;
                    double sY = obj->sY;
                    double sZ = obj->sZ;
                    double a = obj->a;
                    int fX = obj->fX;
                    int fY = obj->fY;
                    int floor_index = obj->item_index;

                    const char* model_id = getModelId(obj_node->Name());

                    fprintf(objs_file, "%d,%d,%d,%d,%s,%f,%f,%f,%f,%f,%f,%f,%d,%d,%d\n",
                            p5d_id,floor_num+1,room_num+1,object_num+1,model_id,
                            x,y,z,sX,sY,sZ,a,fX,fY,floor_index);
                }

                double room_area = node->Area();

                P5DRoom *p5d_room = (P5DRoom*) node->Data();
                int floor_index = p5d_room->item_index;

                if (p5d_room->rtypeStr) {
                    std::string s(p5d_room->rtypeStr); 
                    std::string delimiter = ",";
                    size_t pos = 0;
                    while ((pos = s.find(delimiter)) != std::string::npos) {
                        std::string current_rtype = s.substr(0, pos);
                        int type_id = getRTypeId(current_rtype.c_str());
                        s.erase(0, pos + delimiter.length());
                        fprintf(rtypes_file, "%d,%d,%d,%d,%d\n",
                                p5d_id,floor_num+1,room_num+1,floor_index,type_id);
                    }
                }

                fprintf(rooms_file, "%d,%d,%d,%d,%f,%d\n",
                        p5d_id,floor_num+1,room_num+1,nObjectsRoom,room_area,floor_index);
                
                nObjsFloor += nObjectsRoom;
            }

        }

        double floor_area = floor->Area();
    
        fprintf(floors_file, "%d,%d,%d,%d,%f\n",
                p5d_id,floor_num+1,nRooms,nObjsFloor,floor_area);

        nTotalRooms += nRooms;
        nTotalObjs += nObjsFloor;
    }

    fprintf(scenes_file, "%d,%d,%d,%d,%s\n", 
            p5d_id,nFloors,nTotalRooms,nTotalObjs,name);
    


    // Return OK status
    return 1;
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
            if (!output_directory) output_directory = *argv;
            else { fprintf(stderr, "Invalid program argument: %s", *argv); exit(1); }
            argv++; argc--;
        }
    }

    // Check input filename
    if (!output_directory) {
        fprintf(stderr, "Usage: p5d2csv output_directory [options]\n");
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
    char output_scenes_filename[1024];
    sprintf(output_scenes_filename, "%s/scenes.csv", output_directory);

    char output_floors_filename[1024];
    sprintf(output_floors_filename, "%s/floors.csv", output_directory);

    char output_rooms_filename[1024];
    sprintf(output_rooms_filename, "%s/rooms.csv", output_directory);

    char output_objects_filename[1024];
    sprintf(output_objects_filename, "%s/objects.csv", output_directory);

    char output_room_types_filename[1024];
    sprintf(output_room_types_filename, "%s/room_types.csv", output_directory);

    // Open the csv files
    FILE *scenes_fp = fopen(output_scenes_filename, "w");
    if (!scenes_fp) {
        fprintf(stderr, "Unable to open output file %s\n", output_scenes_filename);
        exit(-1);
    }
    FILE *floors_Fp = fopen(output_floors_filename, "w");
    if (!floors_Fp) {
        fprintf(stderr, "Unable to open output file %s\n", output_floors_filename);
        exit(-1);
    }
    FILE *rooms_fp = fopen(output_rooms_filename, "w");
    if (!rooms_fp) {
        fprintf(stderr, "Unable to open output file %s\n", output_rooms_filename);
        exit(-1);
    }
    FILE *objs_fp = fopen(output_objects_filename, "w");
    if (!objs_fp) {
        fprintf(stderr, "Unable to open output file %s\n", output_objects_filename);
        exit(-1);
    }
    FILE *rtypes_fp = fopen(output_room_types_filename, "w");
    if (!rtypes_fp) {
        fprintf(stderr, "Unable to open output file %s\n", output_room_types_filename);
        exit(-1);
    }

    if(!LoadProjects()) exit(-1);
    if(!LoadRTypes()) exit(-1);

    // Allocate scene
    R3Scene *scene = new R3Scene();
    if (!scene) {
        fprintf(stderr, "Unable to allocate scene.\n");
        exit(-1);
    } 
    if (!scene->ReadPlanner5DFile("project.json")) {
        fprintf(stderr, "Unable to read Planner5D file  %s\n", "project.json");
        exit(-1);
    }

    ParseScene(scene, scenes_fp, floors_Fp, rooms_fp, objs_fp, rtypes_fp);

    // Return success 
    return 0;
}
