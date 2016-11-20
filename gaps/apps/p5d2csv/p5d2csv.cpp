// Source file for the scene converter program



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

static const char *input_project_name = NULL;

static const char *project_list = "../../../data/list-of-projects.txt";
static const char *model_list = "../../../data/list-of-models.txt";
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

static int LoadModelIds() {
    std::ifstream file(model_list);
    std::string str;
    while (std::getline(file, str))
    {
        model_ids.push_back(str);
    }

    return 1;
}

static int 
getRTypeId(const char* name)
{
    for (int i = 0; i < rtype_ids.size(); i++) {
        if (strcmp(rtype_ids[i].c_str(), name) == 0) return i;
    }

    return -1;
}

static int 
getP5DId(const char* name)
{
    for (int i = 0; i < project_ids.size(); i++) {
        if (strcmp(project_ids[i].c_str(), name) == 0) return i;
    }

    return -1;
}

static char*
getModelId(const char* object_name) {
    char* door_id = strstr(object_name, "Door");
    if (door_id)
        return strlen("Door_") + door_id;

    char* window_id = strstr(object_name, "Window");
    if (window_id)
        return strlen("Window_") + window_id;

    char* model_id = strstr(object_name, "Ns_") + strlen("Ns_");
    return model_id;
}

////////////////////////////////////////////////////////////////////////
// HASHING (Unique identifiers)
////////////////////////////////////////////////////////////////////////

/*
 * static size_t
hash(std::string text) {
    std::hash<std::string> hash_func;
    return hash_func(text);
}

static size_t
getUniqueFloorID(int p5d_id, int floor) {
    std::string p5d_id_str = std::to_string(p5d_id);
    std::string floor_str = std::to_string(floor);
    std::string text = p5d_id_str + floor_str;
    
    return hash(text);
}

static size_t
getUniqueRoomID(int p5d_id, int floor, int room) {
    std::string p5d_id_str = std::to_string(p5d_id);
    std::string floor_str = std::to_string(floor);
    std::string room_str = std::to_string(room);

    std::string text = p5d_id_str + floor_str + room_str;
    return hash(text);
}

static size_t
getUniqueObjectID(int p5d_id, int floor, int room, int object) {
    std::string p5d_id_str = std::to_string(p5d_id);
    std::string floor_str = std::to_string(floor);
    std::string room_str = std::to_string(room);
    std::string object_str = std::to_string(object);

    std::string text = p5d_id_str + floor_str + room_str + object_str;
    return hash(text);
} 
*/
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

        char* model_id = getModelId(obj_node->Name());

        fprintf(objs_file, "%d,%d,%d,%d,%s,%f,%f,%f,%f,%f,%f,%f,%d,%d,%d\n",
                p5d_id,floor_num+1,room_num+1,object_num+1,model_id,
                x,y,z,sX,sY,sZ,a,fX,fY,floor_index);
        
        return 1;
}




////////////////////////////////////////////////////////////////////////
// MAIN LOOP
////////////////////////////////////////////////////////////////////////

static int
ParseScene(R3Scene *scene, FILE* scenes_file, FILE* floors_file, FILE* rooms_file, FILE* objs_file )
{
    fprintf(scenes_file, "id,num_floors,num_rooms,num_objects,hash\n");
    fprintf(floors_file, "scene_id,floor_num,num_rooms,num_objects,area\n");
    fprintf(rooms_file, "scene_id,floor_num,room_num,num_objects,area,room_type_id,floor_index\n");
    fprintf(objs_file, "scene_id,floor_num,room_num,object_num,model_id,x,y,z,sX,sY,sZ,a,fX,fY,floor_index\n");
    
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


        int nRooms = floor->NChildren();
        nTotalRooms += nRooms;

        // Rooms
        for (int room_num = 0; room_num < nRooms; room_num++) {
            R3SceneNode* room = floor->Child(room_num);
            
           if (strstr(room->Name(), "Object")) {
                PrintOutdoorObject(objs_file, room, p5d_id, floor_num);
                continue;
            }
 
            int nObjects = room->NChildren();

            // Objects
            for (int object_num = 0; object_num < nObjects; object_num++) {
                R3SceneNode* obj_node = room->Child(object_num);

                P5DObject *obj = (P5DObject*) obj_node->Data();
                if (!obj) // Floors, Walls, etc.
                    continue;

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

                char* model_id = getModelId(obj_node->Name());

                fprintf(objs_file, "%d,%d,%d,%d,%s,%f,%f,%f,%f,%f,%f,%f,%d,%d,%d\n",
                        p5d_id,floor_num+1,room_num+1,object_num+1,model_id,
                        x,y,z,sX,sY,sZ,a,fX,fY,floor_index);
            }


            if (strstr(room->Name(), "Room")) {
                nObjsFloor += nObjects;
                nTotalObjs += nObjects;

                double room_area = room->Area();

                P5DRoom *p5d_room = (P5DRoom*) room->Data();
                int type_id = getRTypeId(p5d_room->rtypeStr);
                int floor_index = room->floor_index;

                fprintf(rooms_file, "%d,%d,%d,%d,%f,%d,%d\n",
                        p5d_id,floor_num+1,room_num+1,nObjects,room_area,type_id,floor_index);
            }
        }

        double floor_area = floor->Area();
    
        fprintf(floors_file, "%d,%d,%d,%d,%f\n",
                p5d_id,floor_num+1,nRooms,nObjsFloor,floor_area);
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
    const char* scenes_file = "scenes.csv";
    const char* floors_file = "floors.csv";
    const char* rooms_file  = "rooms.csv";
    const char* objs_file   = "objects.csv";

    // Open the csv files
    FILE *scenes_fp = fopen(scenes_file, "w");
    if (!scenes_fp) {
        fprintf(stderr, "Unable to open output file %s\n", floors_file);
        exit(-1);
    }
    FILE *floors_Fp = fopen(floors_file, "w");
    if (!floors_Fp) {
        fprintf(stderr, "Unable to open output file %s\n", floors_file);
        exit(-1);
    }
    FILE *rooms_fp = fopen(rooms_file, "w");
    if (!rooms_fp) {
        fprintf(stderr, "Unable to open output file %s\n", rooms_file);
        exit(-1);
    }
    FILE *objs_fp = fopen(objs_file, "w");
    if (!objs_fp) {
        fprintf(stderr, "Unable to open output file %s\n", objs_file);
        exit(-1);
    }

    if(!LoadProjects()) exit(-1);
    if(!LoadRTypes()) exit(-1);

    // Allocate scene
    
    R3Scene *scene = new R3Scene();
    if (!scene) {
        fprintf(stderr, "Unable to allocate scene for %s\n", input_project_name);
        exit(-1);
    } 
    if (!scene->ReadPlanner5DFile("project.json")) {
        fprintf(stderr, "Unable to read Planner5D file  %s\n", "project.json");
        exit(-1);
    }

    ParseScene(scene, scenes_fp, floors_Fp, rooms_fp, objs_fp);

    // Return success 
    return 0;
}
