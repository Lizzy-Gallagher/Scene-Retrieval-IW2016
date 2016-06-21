#ifndef __P5D__
#define __P5D__



////////////////////////////////////////////////////////////////////////
// Dependency include files
////////////////////////////////////////////////////////////////////////

#include <vector>



////////////////////////////////////////////////////////////////////////
// P5D Type definitions
////////////////////////////////////////////////////////////////////////

struct P5DWall {
public:
  P5DWall(struct P5DRoom *room = NULL);
  ~P5DWall(void);
  void Print(FILE *fp = NULL) const;
public:
  struct P5DRoom *room;
  int room_index;
  double x1, y1;
  double x2, y2;
  double w;
  bool hidden;
  void *data;
};

struct P5DObject {
public:
  P5DObject(struct P5DFloor *floor = NULL);
  ~P5DObject(void);
  void Print(FILE *fp = NULL) const;
public:
  struct P5DFloor *floor;
  int floor_index;
  struct P5DRoom *room;
  int room_index;
  char *className;
  char *id;
  double x;
  double y;
  double z;
  double sX;
  double sY;
  double sZ;
  double a;
  int fX;
  int fY;
  int otf;
  int idx_index;
  void *data;
};

struct P5DRoom {
  P5DRoom(struct P5DFloor *floor = NULL);
  ~P5DRoom(void);
  int NWalls(void) const;
  P5DWall *Wall(int k) const;
  void Print(FILE *fp = NULL) const;
public:
  struct P5DFloor *floor;
  int floor_index;
  std::vector<P5DWall *> walls;
  std::vector<P5DObject *> objects;
  char *className;
  double h;
  double x, y;
  double sX, sY;
  char *rtype;
  char *texture;
  char *otexture;
  char *rtexture;
  char *wtexture;
  int idx_index;
  void *data;
};

struct P5DFloor {
  P5DFloor(struct P5DProject *project = NULL);
  ~P5DFloor(void);
  int NRooms(void) const;
  P5DRoom *Room(int k) const;
  int NObjects(void) const;
  P5DObject *Object(int k) const;
  void Print(FILE *fp = NULL) const;
public:
  struct P5DProject *project;
  int project_index;
  std::vector<P5DRoom *> rooms;
  std::vector<P5DObject *> objects;
  double h;
  void *data;
};

struct P5DProject {
  P5DProject(void);
  ~P5DProject(void);
  int NFloors(void) const;
  P5DFloor *Floor(int k) const;
  int ReadFile(const char *filename);
  void Print(FILE *fp = NULL) const;
public:
  char *name;
  std::vector<P5DFloor *> floors;
  void *data;
};



////////////////////////////////////////////////////////////////////////
// INLINE FUNCTIONS
////////////////////////////////////////////////////////////////////////

inline int P5DRoom::
NWalls(void) const
{
  // Return number of walls
  return walls.size();
}


inline P5DWall *P5DRoom::
Wall(int k) const
{
  // Return kth wall
  return walls[k];
}



inline int P5DFloor::
NRooms(void) const
{
  // Return number of rooms
  return rooms.size();
}


inline P5DRoom *P5DFloor::
Room(int k) const
{
  // Return kth room
  return rooms[k];
}



inline int P5DFloor::
NObjects(void) const
{
  // Return number of objects
  return objects.size();
}


inline P5DObject *P5DFloor::
Object(int k) const
{
  // Return kth object
  return objects[k];
}



inline int P5DProject::
NFloors(void) const
{
  // Return number of floors
  return floors.size();
}


inline P5DFloor *P5DProject::
Floor(int k) const
{
  // Return kth floor
  return floors[k];
}





#endif
