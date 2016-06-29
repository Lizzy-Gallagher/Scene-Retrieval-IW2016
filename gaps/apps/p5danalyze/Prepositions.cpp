#include "R3Graphics/R3Graphics.h"
#include "StatsAux.h"
#include "Prepositions.h"
#include "R2Aux.h"
#include "BitEncoding.h"

#include <map>
#include <string>
#include <vector>

using PrepMap = std::map<std::string, std::map<std::string, PrepositionStats>>;

void UpdatePrepositions(R3Scene* scene, std::vector<R3SceneNode*> objects,
        PrepMap* prep_map, FrequencyStats freq_stats, Id2CatMap* id2cat)
{
    // Populate category->category map
    for (int i = 0; i < objects.size(); i++) {
        R3SceneNode* pri_node = objects[i];
        std::string pri_cat = GetObjectCategory(pri_node, id2cat);
        if (pri_cat.size() == 0) continue; 
        //if (pri_cat != only_category) continue;

        (*freq_stats.cat_count)[pri_cat]++;

        for (int j = 0; j < objects.size(); j++) {
            if (i == j) continue;

            R3SceneNode* ref_node = objects[j];
            std::string ref_cat = GetObjectCategory(ref_node, id2cat);
            if (ref_cat.size() == 0) continue;
            
            if ((*prep_map)[pri_cat].count(ref_cat) == 0)
                (*prep_map)[pri_cat][ref_cat] = {}; 
        }

        if ((*prep_map)[pri_cat].count("wall") == 0)
            (*prep_map)[pri_cat]["wall"] = {};
    }
}

R3Affine PrepareWorldToGridXfrom(R3Point cen_pnt, DrawingValues values) {
    R3Vector cen = R3Vector(cen_pnt.X(), cen_pnt.Y(), cen_pnt.Z());

   // Start with the identity matrix
   R3Affine world_to_grid_xform = R3identity_affine;

    // Transform the dist3Dance from src_node
    if (values.dist3D != NULL) {
        values.dist3D->Rotate(R3posz_vector, -1.0 * values.theta);
        if (values.do_fX) values.dist3D->Mirror(R3posyz_plane);
        if (values.do_fY) values.dist3D->Mirror(R3posxz_plane);
        world_to_grid_xform.Translate(*(values.dist3D) /* * pixels_to_meters*/);
        world_to_grid_xform.Translate(-1.0 * *(values.dist3D));
    }
    
    // Center
    world_to_grid_xform.Translate(values.translation3D);

    // Transform about the origin
    world_to_grid_xform.Translate(cen);
    if (values.do_fX) world_to_grid_xform.XMirror();
    if (values.do_fY) world_to_grid_xform.YMirror();
    //world_to_grid_xform.Scale(pixels_to_meters);
    world_to_grid_xform.Rotate(R3posz_vector, -1.0 * values.theta);
    world_to_grid_xform.Translate(-1.0 * cen);

    return world_to_grid_xform;
}



R3Grid* DrawObject(R3SceneNode* node, R3Grid *grid)
{
    R3Grid temp_grid = R3Grid(grid->XResolution(), grid->YResolution(), grid->ZResolution());

    DrawingValues values = CreateDrawingValues(node, grid->XResolution());
    R3Affine world_to_grid_xform = PrepareWorldToGridXfrom(node->Centroid(), values);
    
    // For all R3SceneElements in the R3SceneNode
    for (int k = 0; k < node->NElements(); k++) {
        R3SceneElement* el = node->Element(k);

        // For all R3Shapes in the R3SceneElements    
        for (int l = 0; l < el->NShapes(); l++) {

            R3Shape* shape = el->Shape(l);
            R3TriangleArray* arr = (R3TriangleArray*) shape;

            // For all R3Triangles in the R3TriangleArray
            for (int t = 0; t < arr->NTriangles(); t++) {
                R3Triangle *triangle = arr->Triangle(t);

                // Create new points
                R3Point v0 = R3Point(triangle->V0()->Position());
                R3Point v1 = R3Point(triangle->V1()->Position());
                R3Point v2 = R3Point(triangle->V2()->Position());

                if (IsOutsideGrid(&temp_grid, v0, v1, v2)) continue;

                // Move exterior verticies inside the grid
                std::vector<R3Point> v = MoveInsideGrid(&temp_grid, v0, v1, v2);
                temp_grid.RasterizeWorldTriangle(v[0], v[1], v[2], 1);
            }
        }
    }

    // Color the shape with a single color
    temp_grid.Threshold(0, 0, 1);
    grid->Add(temp_grid);
    return grid;
}

void PrintSide(R3Box& side, const char* name) {
    fprintf(stdout, "\n\t%s \n\t\t(%f, %f, %f) (%f, %f, %f) \n\t\t(%f, %f, %f) (%f, %f, %f)\n",
           name,
           side.Corner(0).X(), side.Corner(0).Y(), side.Corner(0).Z(),
           side.Corner(1).X(), side.Corner(1).Y(), side.Corner(1).Z(),
           side.Corner(5).X(), side.Corner(5).Y(), side.Corner(5).Z(),
           side.Corner(6).X(), side.Corner(6).Y(), side.Corner(6).Z());
}

void PrintBox(R3Box side, const char* name) {
    fprintf(stderr, "\n\t%s \n\t\t(%f, %f, %f) (%f, %f, %f) \n\t\t(%f, %f, %f) (%f, %f, %f)"
                    "\n\t\t(%f, %f, %f) (%f, %f, %f) \n\t\t(%f, %f, %f) (%f, %f, %f)\n",
           name,
           side.Corner(0).X(), side.Corner(0).Y(), side.Corner(0).Z(),
           side.Corner(1).X(), side.Corner(1).Y(), side.Corner(1).Z(),
           side.Corner(2).X(), side.Corner(2).Y(), side.Corner(2).Z(),
           side.Corner(3).X(), side.Corner(3).Y(), side.Corner(3).Z(),
           side.Corner(4).X(), side.Corner(4).Y(), side.Corner(4).Z(),
           side.Corner(5).X(), side.Corner(5).Y(), side.Corner(5).Z(),
           side.Corner(6).X(), side.Corner(6).Y(), side.Corner(6).Z(),
           side.Corner(7).X(), side.Corner(7).Y(), side.Corner(7).Z());
}

R3Box CalcSide(R3Box bb, R3Grid& grid, int prep) {
    R3Box side;
    switch (prep) {
        case PREP_FRONTSIDE: side = bb.Side(RN_HY_SIDE); break;
        case PREP_BACKSIDE:  side = bb.Side(RN_LY_SIDE); break;
        case PREP_ABOVE:     side = bb.Side(RN_HZ_SIDE); break;
        case PREP_BELOW:     side = bb.Side(RN_LZ_SIDE); break;
        case PREP_RIGHTSIDE: side = bb.Side(RN_HX_SIDE); break;
        case PREP_LEFTSIDE:  side = bb.Side(RN_LX_SIDE); break;
    }
    
    int meters_of_context = 5;

    R3Point min = R3Point(side.XMin(), side.YMin(), side.ZMin());
    R3Point max = R3Point(side.XMax(), side.YMax(), side.ZMax());

    switch (prep) {
        case PREP_FRONTSIDE: max.SetY(max.Y() + meters_of_context); break;
        case PREP_BACKSIDE: min.SetY(min.Y() - meters_of_context); break;
        case PREP_ABOVE : max.SetZ(max.Z() + meters_of_context); break;
        case PREP_BELOW: min.SetZ(min.Z() - meters_of_context); break;
        case PREP_RIGHTSIDE : max.SetX(max.X() + meters_of_context) ; break;
        case PREP_LEFTSIDE : min.SetX(min.X() - meters_of_context); break;
    }

    //R3Box preposition_region = R3Box(min, max);
    return R3Box(min, max) ;
}

const char* prep_names[] = {
    "FRONTSIDE",
    "BACKSIDE",
    "ABOVE",
    "BELOW",
    "RIGHTSIDE",
    "LEFTSIDE",
};

std::map<int, PrepRegion> InitPrepositions(R3SceneNode* node) {
    //BitEncoding be = BitEncoding(NUM_PREPOSITIONS);
    std::map<int, PrepRegion> prep_region_map;

    int resolution = 50;

    // Create a grid that encodes information about the points
    R3Grid grid = R3Grid(resolution, resolution, resolution);
   
    DrawingValues values = CreateDrawingValues(node, resolution);
    R3Affine world_to_grid_xform = PrepareWorldToGridXfrom(node->Centroid(), values);
  
    R3Box bb = node->BBox();
    bb.Transform(world_to_grid_xform);

    for (int prep_int = 0; prep_int < NUM_PREPOSITIONS; ++prep_int) {
        R3Box region = CalcSide(bb, grid, prep_int);
        PrepRegion pr = { prep_names[prep_int], prep_int, region };
        prep_region_map[prep_int] = pr;
 //       PrintBox(prep_region_map[prep_int]->Region(), prep_names[prep_int]);

        //grid.RasterizeGridBox(region, be.GetRasterizationValue(prep_int));      
    }
//exit(0);
    fprintf(stdout, "Finished InitPrepositions\n");
    return prep_region_map;
}


R3Box* Intersect(R3Box b1, R3Box b2) {
    R3Box* area_of_intersection = NULL;

    // Check if any Intersect
    if (b1.XMax() <= b2.XMin() || b2.XMax() <= b1.XMin())
      return area_of_intersection;
    if (b1.YMax() <= b2.YMin() || b2.YMax() <= b1.YMin())
      return area_of_intersection;
    if (b1.ZMax() <= b2.ZMin() || b2.ZMax() <= b1.ZMin())
      return area_of_intersection;

    // Now compute area of intersection
    float xMin = std::max(b1.XMin(), b2.XMin());
    float yMin = std::max(b1.YMin(), b2.YMin());
    float zMin = std::max(b1.ZMin(), b2.ZMin());

    float xMax = std::min(b1.XMax(), b2.XMax());
    float yMax = std::min(b1.YMax(), b2.YMax());
    float zMax = std::min(b1.ZMax(), b2.ZMax());

    area_of_intersection = new R3Box(xMin, yMin, zMin, xMax, yMax, zMax);
    return area_of_intersection;
}

void CalcPrepositions(R3SceneNode* pri_obj, R3SceneNode* ref_obj, std::string pri_cat, 
        std::string ref_cat, Id2CatMap* id2cat, std::map<int, PrepRegion> prep_region_map) {
    R3Box ref_bb = ref_obj->BBox();

    // Create and Apply Transform to put nearby region
    DrawingValues values = CreateDrawingValues(pri_obj, 50);
    R3Vector dist3d = (ref_obj->Centroid() - pri_obj->Centroid());
    values.dist3D = &dist3d;  // TODO: fix 
    R3Affine world_to_grid_xform = PrepareWorldToGridXfrom(ref_obj->Centroid(), values);
    ref_bb.Transform(world_to_grid_xform);

    fprintf(stdout, "In CalcPrepositions\n");
    for (int i = 0; i < prep_region_map.size(); ++i) {
        //Prep prep = static_cast<Prep>(i);
        PrepRegion pr = prep_region_map[i];
        R3Box region = pr.region;
        const char *name = pr.name;

        R3Box* result = Intersect(region, ref_bb);
        if (result == NULL)
            continue;
        
        float volume_of_overlap = result->Volume() / ref_bb.Volume();

        /*fprintf(stdout, "\tPrep: %s, Old Area: %f, New Area: %f, Region Area: %f, Percent: %f %% \n", 
                prep_names[i], ref_bb.Volume(), result->Volume(), region.Volume(),
                (result->Volume() / ref_bb.Volume()) * 100);*/

    }
    exit(1);

}

