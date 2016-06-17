/* Source file for the R3 surfel scan class */



////////////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////////////

#include "R3Surfels/R3Surfels.h"



////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS/DESTRUCTORS
////////////////////////////////////////////////////////////////////////

R3SurfelScan::
R3SurfelScan(const char *name)
  : scene(NULL),
    scene_index(-1),
    node(NULL),
    pose(R3Point(0,0,0), R3Triad(R3Vector(0,0,0), R3Vector(0,0,0))),
    focal_length(0),
    timestamp(0),
    width(0), 
    height(0),
    name((name) ? strdup(name) : NULL),
    flags(0),
    data(NULL)
{
}



R3SurfelScan::
R3SurfelScan(const R3SurfelScan& scan)
  : scene(NULL),
    scene_index(-1),
    node(NULL),
    pose(scan.pose),
    focal_length(scan.focal_length),
    timestamp(scan.timestamp),
    width(scan.width), 
    height(scan.height),
    name((scan.name) ? strdup(scan.name) : NULL),
    flags(0),
    data(NULL)
{
}



R3SurfelScan::
~R3SurfelScan(void)
{
  // Remove node
  if (node) SetNode(NULL);

  // Delete scan from scene
  if (scene) scene->RemoveScan(this);

  // Delete name
  if (name) free(name);
}



////////////////////////////////////////////////////////////////////////
// POINT ACCESS FUNCTIONS
////////////////////////////////////////////////////////////////////////

R3SurfelPointSet *R3SurfelScan::
PointSet(void) const
{
  // Get/check node
  R3SurfelNode *node = Node();
  if (!node) return NULL;

  // Allocate point set
  R3SurfelPointSet *pointset = new R3SurfelPointSet();
  if (!pointset) {
    fprintf(stderr, "Unable to allocate point set\n");
    return NULL;
  }

  // Insert points from all blocks
  for (int j = 0; j < node->NBlocks(); j++) {
    R3SurfelBlock *block = node->Block(j);
    pointset->InsertPoints(block);
  }
  
  // Return pointset
  return pointset;
}



////////////////////////////////////////////////////////////////////////
// PROPERTY MANIPULATION FUNCTIONS
////////////////////////////////////////////////////////////////////////

void R3SurfelScan::
SetPose(const R3CoordSystem& pose) 
{
  // Set pose
  this->pose = pose;
}



void R3SurfelScan::
SetViewpoint(const R3Point& viewpoint) 
{
  // Set viewpoint
  this->pose.SetOrigin(viewpoint);
}



void R3SurfelScan::
SetOrientation(const R3Vector& towards, const R3Vector& up) 
{
  // Set orientation
  this->pose.SetAxes(R3Triad(towards, up));
}



void R3SurfelScan::
SetFocalLength(RNLength f) 
{
  // Set timestamp
  this->focal_length = f;
}



void R3SurfelScan::
SetTimestamp(RNScalar timestamp) 
{
  // Set timestamp
  this->timestamp = timestamp;
}



void R3SurfelScan::
SetName(const char *name)
{
  // Delete previous name
  if (this->name) free(this->name);
  this->name = (name) ? strdup(name) : NULL;

  // Mark scene as dirty
  if (Scene()) Scene()->SetDirty();
}



void R3SurfelScan::
SetResolution(int width, int height) 
{
  // Set resolution
  this->width = width;
  this->height = height;
}



void R3SurfelScan::
SetData(void *data) 
{
  // Set user data
  this->data = data;
}



////////////////////////////////////////////////////////////////////////
// BLOCK MANIPULATION FUNCTIONS
////////////////////////////////////////////////////////////////////////

void R3SurfelScan::
SetNode(R3SurfelNode *node)
{
  // Check if the same
  if (this->node == node) return;

  // Update old node
  if (this->node) {
    assert(this->node->scan == this);
    this->node->scan = NULL;
  }

  // Update new node
  if (node) {
    assert(node->scan == NULL);
    node->scan = this;
  }

  // Assign node
  this->node = node;

  // Mark scene as dirty
  if (Scene()) Scene()->SetDirty();
}



////////////////////////////////////////////////////////////////////////
// DISPLAY FUNCTIONS
////////////////////////////////////////////////////////////////////////

void R3SurfelScan::
Print(FILE *fp, const char *prefix, const char *suffix) const
{
  // Check fp
  if (!fp) fp = stdout;

  // Print scan
  if (prefix) fprintf(fp, "%s", prefix);
  fprintf(fp, "%d %s %d", SceneIndex(), (Name()) ? Name() : "-", (Node()) ? node->TreeIndex() : -1);
  if (suffix) fprintf(fp, "%s", suffix);
  fprintf(fp, "\n");
}




void R3SurfelScan::
Draw(RNFlags flags) const
{
  // Draw node
  if (node) node->Draw(flags);
}



