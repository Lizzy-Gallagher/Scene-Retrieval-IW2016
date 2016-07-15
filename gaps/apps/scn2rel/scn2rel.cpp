// Source file for the scene converter program



// Include files 

#include "R3Graphics/R3Graphics.h"



// Program arguments

static const char *input_scene_name = NULL;
static const char *output_relationship_name = NULL;
static double max_radius = 5;
static double grid_spacing = 0.05;
static int grid_max_resolution = 256;
static int print_verbose = 0;



// Region constants

enum {
  BELOW_BBOX_X,       
  WITHIN_BBOX_X,
  ABOVE_BBOX_X,       
  BELOW_BBOX_Y,       
  WITHIN_BBOX_Y,
  ABOVE_BBOX_Y,       
  BELOW_BBOX_Z,
  WITHIN_BBOX_Z,
  ABOVE_BBOX_Z,
  NUM_BBOX_REGIONS
};

enum {
  BELOW_PROJECTION_X,       
  WITHIN_PROJECTION_X,
  ABOVE_PROJECTION_X,       
  BELOW_PROJECTION_Y,       
  WITHIN_PROJECTION_Y,
  ABOVE_PROJECTION_Y,       
  BELOW_PROJECTION_Z,
  WITHIN_PROJECTION_Z,
  ABOVE_PROJECTION_Z,       
  NUM_PROJECTION_REGIONS
};



////////////////////////////////////////////////////////////////////////
// I/O STUFF
////////////////////////////////////////////////////////////////////////

static R3Scene *
ReadScene(const char *filename)
{
  // Start statistics
  RNTime start_time;
  start_time.Read();

  // Allocate scene
  R3Scene *scene = new R3Scene();
  if (!scene) {
    fprintf(stderr, "Unable to allocate scene for %s\n", filename);
    return NULL;
  }

  // Read scene from file
  if (!scene->ReadFile(filename)) {
    delete scene;
    return NULL;
  }

  // Print statistics
  if (print_verbose) {
    printf("Read scene from %s ...\n", filename);
    printf("  Time = %.2f seconds\n", start_time.Elapsed());
    printf("  # Nodes = %d\n", scene->NNodes());
    fflush(stdout);
  }

  // Return scene
  return scene;
}



static R3Box
LocalBBox(R3SceneNode *node)
{
  // Compute bounding box in local coordinates
  R3Box bbox = R3null_box;
  for (int i = 0; i < node->NElements(); i++) {
    R3SceneElement *element = node->Element(i);
    bbox.Union(element->BBox());
  }
  return bbox;
}



static R3Affine
LocalToWorldTransformation(R3SceneNode *node)
{
  // Compute transformation from local to global coordinates
  R3Affine transformation = R3identity_affine;
  R3SceneNode *ancestor = node;
  while (ancestor) {
    R3Affine tmp = R3identity_affine;
    tmp.Transform(ancestor->Transformation());
    tmp.Transform(transformation);
    transformation = tmp;
    ancestor = ancestor->Parent();
  }

  // Return transformation
  return transformation;
}



static R3Grid *
CreateSurfaceGrid(R3SceneNode *node, R3Box bbox, RNScalar scale)
{
  // Scale parameters
  RNLength radius = max_radius;
  RNLength spacing = grid_spacing;
  if (RNIsNotZero(scale) && RNIsNotEqual(scale, 1.0)) {
    radius /= scale;
    spacing /= scale;
  }
    
  // Inflate bounding box
  if (radius > 0) {
    bbox[0] -= R3Vector(radius, radius, radius);
    bbox[1] += R3Vector(radius, radius, radius);
  }

  // Compute grid spacing
  RNLength diameter = bbox.LongestAxisLength();
  RNLength min_spacing = (grid_max_resolution > 0) ? diameter / grid_max_resolution : RN_EPSILON;
  if (spacing == 0) spacing = diameter / 64;
  if (spacing < min_spacing) spacing = min_spacing;

  // Compute grid resolution
  int xres = (int) (bbox.XLength() / spacing + 0.5); if (xres == 0) xres = 1;
  int yres = (int) (bbox.YLength() / spacing + 0.5); if (yres == 0) yres = 1;
  int zres = (int) (bbox.ZLength() / spacing + 0.5); if (zres == 0) zres = 1;
    
  // Allocate grid
  R3Grid *grid = new R3Grid(xres, yres, zres, bbox);
  if (!grid) {
    fprintf(stderr, "Unable to allocate grid\n");
    return NULL;
  }

  // Rasterize scene into grid
  for (int i = 0; i < node->NElements(); i++) {
    R3SceneElement *element = node->Element(i);
    for (int j = 0; j < element->NShapes(); j++) {
      R3Shape *shape = element->Shape(j);
      if (shape->ClassID() == R3TriangleArray::CLASS_ID()) {
        R3TriangleArray *triangles = (R3TriangleArray *) shape;
        for (int k = 0; k < triangles->NTriangles(); k++) {
          R3Triangle *triangle = triangles->Triangle(k);
          R3TriangleVertex *v0 = triangle->V0();
          R3TriangleVertex *v1 = triangle->V1();
          R3TriangleVertex *v2 = triangle->V2();
          const R3Point& p0 = v0->Position();
          const R3Point& p1 = v1->Position();
          const R3Point& p2 = v2->Position();
          grid->RasterizeWorldTriangle(p0, p1, p2, 1.0);
        }
      }
    }
  }

  // Threshold grid (to compensate for possible double rasterization)
  grid->Threshold(0.5, 0.0, 1.0);
 
  // Return grid
  return grid;
}



static R3PlanarGrid *
CreatePlanarGrids(const R3Grid& surface_grid)
{
  // Allocate array of planar grids
  R3PlanarGrid *planar_grids = new R3PlanarGrid [ 6 ];

  // Create grids
  for (int dir = RN_LO; dir <= RN_HI; dir++) {
    for (int dim = RN_X; dim <= RN_Z; dim++) {
      int dim1 = (dim+1)%3;
      int dim2 = (dim+2)%3;
      int side = 3*dir + dim;
      int coord[3];

      // Create planar grid
      RNScalar spacing = surface_grid.GridToWorldScaleFactor();
      R3Box world_box = surface_grid.WorldBox();
      R3Plane plane = world_box.Plane(dir, dim); plane.Flip();
      R3Box side_box = world_box.Side(dir, dim);
      planar_grids[side].Reset(plane, side_box, spacing);
      planar_grids[side].Clear(R2_GRID_UNKNOWN_VALUE);

      // Fill planar grid
      for (coord[dim1] = 0; coord[dim1] < surface_grid.Resolution(dim1); coord[dim1]++) {
        for (coord[dim2] = 0; coord[dim2] < surface_grid.Resolution(dim2); coord[dim2]++) {
          for (int i = 0; i < surface_grid.Resolution(dim); i++) {
            coord[dim] = (dir == RN_HI) ? surface_grid.Resolution(dim)-1-i : i;
            RNScalar value = surface_grid.GridValue(coord[0], coord[1], coord[2]);
            if (RNIsNotZero(value)) {
              R3Point world_position = surface_grid.WorldPosition(coord[0]+0.5, coord[1]+0.5, coord[2]+0.5);
              R2Point grid_position = planar_grids[side].GridPosition(world_position);
              int grid_ix = (int) (grid_position.X() + 0.5);               
              if ((grid_ix < 0) || (grid_ix >= planar_grids[side].XResolution())) break;
              int grid_iy = (int) (grid_position.Y() + 0.5);
              if ((grid_iy < 0) || (grid_iy >= planar_grids[side].YResolution())) break;
              planar_grids[side].SetGridValue(grid_ix, grid_iy, world_position[dim]);
              break;
            }
          }
        }
      }

      // char tmp[1024];
      // sprintf(tmp, "s_%d_%d.grd", dir, dim);
      // planar_grids[side].WriteFile(tmp);
    }
  }

  // Return grids
  return planar_grids;
}



static R3Grid *
CreateSquaredDistanceGrid(R3Grid& input)
{
  // Input has 1 on surface, 0 everywhere else
  // Output has squared distance (in local coordinates) to closest surface

  // Allocate the grid
  R3Grid *grid = new R3Grid(input);
  if (!grid) {
    fprintf(stderr, "Unable to allocate squared distance grid\n");
    return NULL;
  }

  // Compute squared distance transform
  grid->SquaredDistanceTransform();

  // Scale to local coordinates (rather than grid coordinates)
  RNScalar scale = grid->GridToWorldScaleFactor() * grid->GridToWorldScaleFactor();
  grid->Multiply(scale);

  // Return grid
  return grid;
}



static int 
WriteRelationships(R3Scene *scene, const char *filename)
{
  // Start statistics
  RNTime start_time;
  start_time.Read();
  int npairs = 0;

  // Open file
  FILE *fp = fopen(filename, "w");
  if (!fp) {
    fprintf(stderr, "Unable to open output file %s\n", filename);
    return 0;
  }

  // Initialize thresholds for histograms of squared distances
  const int NUM_DD_BINS = 10;
  const RNLength start_d = 0.01;
  RNLength ddt[NUM_DD_BINS] = { start_d * start_d };
  for (int i = 1; i < NUM_DD_BINS; i++) ddt[i] = 2.0 * 2.0 * ddt[i-1];

  // Foreach leaf node1
  for (int i1 = 0; i1 < scene->NNodes(); i1++) {
    R3SceneNode *node1 = scene->Node(i1);
    if (node1->NChildren() > 0) continue;
    const char *name1 = node1->Name();
    if (!name1) continue;
    const R3Box& bbox1 = LocalBBox(node1);
    if (bbox1.IsEmpty()) continue;
    R3Affine transformation1 = LocalToWorldTransformation(node1);
    RNScalar scale1 = transformation1.ScaleFactor();

    // Create surface grid for node1
    R3Grid *surface_grid1 = CreateSurfaceGrid(node1, bbox1, scale1);
    if (!surface_grid1) continue;

    // Create squared distance grid for node1
    R3Grid *squared_distance_grid1 = CreateSquaredDistanceGrid(*surface_grid1);
    if (!squared_distance_grid1) { delete surface_grid1; continue; }
    if (RNIsNotEqual(scale1, 1.0)) squared_distance_grid1->Multiply(scale1 * scale1);

    // Create planar grids
    R3PlanarGrid *planar_grids1 = CreatePlanarGrids(*surface_grid1);
    if (!planar_grids1) { delete surface_grid1; delete squared_distance_grid1; continue; }

    // Foreach leaf node2
    for (int i2 = 0; i2 < scene->NNodes(); i2++) {
      R3SceneNode *node2 = scene->Node(i2);
      if (node2 == node1) continue;
      if (node2->NChildren() > 0) continue;
      const char *name2 = node2->Name();
      if (!name2) continue;
      const R3Box& bbox2 = LocalBBox(node2);
      if (bbox2.IsEmpty()) continue;
      R3Affine transformation2 = LocalToWorldTransformation(node2);
      RNScalar scale2 = transformation2.ScaleFactor();

      // Check if nodes are in same room
      if (node1->Parent() != node2->Parent()) continue;

      // Check if nodes are within maximum radius
      R3Box world_bbox1 = bbox1;  world_bbox1.Transform(transformation1);
      R3Box world_bbox2 = bbox2;  world_bbox2.Transform(transformation2);
      RNLength d = R3Distance(world_bbox1, world_bbox2);
      if (d > max_radius) continue;

#define USE_GRID
#ifdef USE_GRID
      // Create grid for node2
      R3Grid *surface_grid2 = CreateSurfaceGrid(node2, bbox2, scale2);
      if (!surface_grid2) continue;
#else
      R3Grid *surface_grid2 = new R3Grid();
      if (!surface_grid2) continue;
#endif
      
      // Initialize relationship statistics
      int npoints = 0;
      int bc[NUM_BBOX_REGIONS], pc[NUM_PROJECTION_REGIONS], ddc[NUM_DD_BINS];
      for (int i = 0; i < NUM_BBOX_REGIONS; i++) bc[i] = 0;
      for (int i = 0; i < NUM_PROJECTION_REGIONS; i++) pc[i] = 0;
      for (int i = 0; i < NUM_DD_BINS; i++) ddc[i] = 0;
      RNScalar closest_dd = FLT_MAX;

      // Compute relationship statistics
      for (int ix = 0; ix < surface_grid2->XResolution(); ix++) {
        for (int iy = 0; iy < surface_grid2->YResolution(); iy++) {
          for (int iz = 0; iz < surface_grid2->ZResolution(); iz++) {
            RNScalar value2 = surface_grid2->GridValue(ix, iy, iz);
            if (value2 <= 0) continue;

            // Get position in node1 local coordinates
            R3Point p = surface_grid2->WorldPosition(R3Point(ix+0.5, iy+0.5, iz+0.5));
            p.Transform(transformation2);
            p.InverseTransform(transformation1);

            // Check if p is in node1 bounding box
            if (!R3Contains(surface_grid1->WorldBox(), p)) continue;

            // Get position in node1 grid coordinates
            R3Point gp = surface_grid1->GridPosition(p);
            int gx = (int) (gp.X() + 0.5);
            if ((gx < 0) || (gx >= surface_grid1->XResolution())) continue;
            int gy = (int) (gp.Y() + 0.5);
            if ((gy < 0) || (gy >= surface_grid1->YResolution())) continue;
            int gz = (int) (gp.Z() + 0.5);
            if ((gz < 0) || (gz >= surface_grid1->ZResolution())) continue;

            // Get squared distance to closest point
            // RNScalar d;
            // R3Point tmp(p);
            // tmp.Transform(node1->Transformation());
            // if (!node1->FindClosest(tmp, NULL, NULL, NULL, NULL, NULL, &d, 0, max_radius)) continue;
            // RNScalar dd = d * d;

#ifdef USE_GRID
            RNScalar dd = squared_distance_grid1->GridValue(gp);
#else                     
            R3Point tmp(p);
            tmp.Transform(node1->Transformation());
            RNScalar d = node1->Distance(tmp);
            RNScalar dd = d * d;
#endif            
            // Update squared distance statistics
            if (dd < closest_dd) closest_dd = dd;
            int bin = 0;
            while (bin < NUM_DD_BINS) {
              if (dd < ddt[bin]) { ddc[bin]++; break; }
              else if (bin == NUM_DD_BINS-1) { ddc[bin]++; break; }
              else bin++;
            }

            // Update bbox region counts
            if (RNIsLess(p.X(), bbox1.XMin())) bc[BELOW_BBOX_X]++;
            else if (RNIsGreater(p.X(), bbox1.XMax())) bc[ABOVE_BBOX_X]++;
            else bc[WITHIN_BBOX_X]++;
            if (RNIsLess(p.Y(), bbox1.YMin())) bc[BELOW_BBOX_Y]++;
            else if (RNIsGreater(p.Y(), bbox1.YMax())) bc[ABOVE_BBOX_Y]++;
            else bc[WITHIN_BBOX_Y]++;
            if (RNIsLess(p.Z(), bbox1.ZMin())) bc[BELOW_BBOX_Z]++;
            else if (RNIsGreater(p.Z(), bbox1.ZMax())) bc[ABOVE_BBOX_Z]++;
            else bc[WITHIN_BBOX_Z]++;

            // Update projection region counts
            RNCoord x1 = planar_grids1[3*RN_LO+RN_X].GridValue(gy, gz);
            RNCoord x2 = planar_grids1[3*RN_HI+RN_X].GridValue(gy, gz);
            RNCoord y1 = planar_grids1[3*RN_LO+RN_Y].GridValue(gz, gx);
            RNCoord y2 = planar_grids1[3*RN_HI+RN_Y].GridValue(gz, gx);
            RNCoord z1 = planar_grids1[3*RN_LO+RN_Z].GridValue(gx, gy);
            RNCoord z2 = planar_grids1[3*RN_HI+RN_Z].GridValue(gx, gy);
            if ((x1 != R2_GRID_UNKNOWN_VALUE) && (RNIsLess(p.X(), x1))) pc[BELOW_PROJECTION_X]++;
            else if ((x2 != R2_GRID_UNKNOWN_VALUE) && (RNIsGreater(p.X(), x2))) pc[ABOVE_PROJECTION_X]++;
            else if ((x1 != R2_GRID_UNKNOWN_VALUE) && (x2 != R2_GRID_UNKNOWN_VALUE)) pc[WITHIN_PROJECTION_X]++;
            if ((y1 != R2_GRID_UNKNOWN_VALUE) && (RNIsLess(p.Y(), y1))) pc[BELOW_PROJECTION_Y]++;
            else if ((y2 != R2_GRID_UNKNOWN_VALUE) && (RNIsGreater(p.Y(), y2))) pc[ABOVE_PROJECTION_Y]++;
            else if ((y1 != R2_GRID_UNKNOWN_VALUE) && (y2 != R2_GRID_UNKNOWN_VALUE)) pc[WITHIN_PROJECTION_Y]++;
            if ((z1 != R2_GRID_UNKNOWN_VALUE) && (RNIsLess(p.Z(), z1))) pc[BELOW_PROJECTION_Z]++;
            else if ((z2 != R2_GRID_UNKNOWN_VALUE) && (RNIsGreater(p.Z(), z2))) pc[ABOVE_PROJECTION_Z]++;
            else if ((z1 != R2_GRID_UNKNOWN_VALUE) && (z2 != R2_GRID_UNKNOWN_VALUE)) pc[WITHIN_PROJECTION_Z]++;

            // Update count of points considered
            npoints++;
          }
        }
      }
      
      // Check relationship statistics
      if (npoints == 0) { delete surface_grid2; continue; }
      if (closest_dd > max_radius * max_radius) { delete surface_grid2; continue; }
      RNScalar point_area = surface_grid1->GridToWorldScaleFactor() * surface_grid1->GridToWorldScaleFactor();

      // Compute centroid statistics
      R3Point c = R3zero_point;
      c.Transform(transformation2);
      c.InverseTransform(transformation1);

      // Write relationship statistics
      fprintf(fp, "%s %s  ", name1, name2);
      fprintf(fp, "%.3g %.3g %d  ", point_area, sqrt(closest_dd), npoints);
      fprintf(fp, "%.3g %.3g %.3g  ", c.X(), c.Y(), c.Z());
      for (int i = 0; i < NUM_DD_BINS; i++) fprintf(fp, "%d ", ddc[i]); fprintf(fp, " ");
      for (int i = 0; i < NUM_BBOX_REGIONS; i++) fprintf(fp, "%d ", bc[i]); fprintf(fp, " ");
      for (int i = 0; i < NUM_PROJECTION_REGIONS; i++) fprintf(fp, "%d ", pc[i]); fprintf(fp, " ");
      fprintf(fp, "\n");

      // Update statistics
      npairs++;
        
      // Delete grid for node2
      delete surface_grid2;
    }

    // Delete grids for node1
    delete [] planar_grids1;
    delete squared_distance_grid1;
    delete surface_grid1;
  }

  // Close file
  fclose(fp);

  // Print statistics
  if (print_verbose) {
    printf("Wrote relationships to %s ...\n", filename);
    printf("  Time = %.2f seconds\n", start_time.Elapsed());
    printf("  # Pairs = %d\n", npairs);
    fflush(stdout);
  }

  // Return success
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
      if (!input_scene_name) input_scene_name = *argv;
      else if (!output_relationship_name) output_relationship_name = *argv;
      else { fprintf(stderr, "Invalid program argument: %s", *argv); exit(1); }
      argv++; argc--;
    }
  }

  // Check input filename
  if (!input_scene_name || !output_relationship_name) {
    fprintf(stderr, "Usage: scn2rel inputscenefile outputrelationshipsfile [options]\n");
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

  // Read scene
  R3Scene *scene = ReadScene(input_scene_name);
  if (!scene) exit(-1);

  // Write relationships
  if (!WriteRelationships(scene, output_relationship_name)) exit(-1);

  // Return success 
  return 0;
}
