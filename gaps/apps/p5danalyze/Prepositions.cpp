#include "R3Graphics/R3Graphics.h"
#include "StatsAux.h"
#include "Prepositions.h"
#include "R2Aux.h"
#include "IOAux.h"

#include <map>
#include <string>
#include <vector>

using PrepMap = std::map<std::string, std::map<std::string, PrepositionStats>>;

/* Debug */

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

/* Helpers */

R3Affine PrepareWorldToGridXfrom(R3Point cen_pnt, DrawingValues values) 
{
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

R3Box CalcSide(R3Box bb, int prep, int meters_of_context) {
    R3Box side;
    switch (prep) {
        case PREP_FRONTSIDE: side = bb.Side(RN_HY_SIDE); break;
        case PREP_BACKSIDE:  side = bb.Side(RN_LY_SIDE); break;
        case PREP_ABOVE:     side = bb.Side(RN_HZ_SIDE); break;
        case PREP_BELOW:     side = bb.Side(RN_LZ_SIDE); break;
        case PREP_RIGHTSIDE: side = bb.Side(RN_HX_SIDE); break;
        case PREP_LEFTSIDE:  side = bb.Side(RN_LX_SIDE); break;
    }
    
    R3Point min = side.Min(); 
    R3Point max = side.Max();

    switch (prep) {
        case PREP_FRONTSIDE: max.SetY(max.Y() + meters_of_context); break;
        case PREP_BACKSIDE:  min.SetY(min.Y() - meters_of_context); break;
        case PREP_ABOVE:     max.SetZ(max.Z() + meters_of_context); break;
        case PREP_BELOW:     min.SetZ(min.Z() - meters_of_context); break;
        case PREP_RIGHTSIDE: max.SetX(max.X() + meters_of_context); break;
        case PREP_LEFTSIDE:  min.SetX(min.X() - meters_of_context); break;
    }

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

void UpdateStats(PrepositionStats& prep_stats, int preposition) {  
    switch (preposition) {
        case PREP_FRONTSIDE: ++prep_stats.frontside; break;
        case PREP_BACKSIDE: ++prep_stats.backside; break;
        case PREP_ABOVE: ++prep_stats.above; break;
        case PREP_BELOW: ++prep_stats.below; break;
        case PREP_RIGHTSIDE: ++prep_stats.rightside; break;
        case PREP_LEFTSIDE: ++prep_stats.leftside; break;
    } 
}

R3Box* CalcIntersection(R3Box b1, R3Box b2) {
    R3Box* area_of_intersection = NULL;

    // Check if any CalcIntersection
    if (b1.XMax() <= b2.XMin() || b2.XMax() <= b1.XMin() ||
        b1.YMax() <= b2.YMin() || b2.YMax() <= b1.YMin() ||
        b1.ZMax() <= b2.ZMin() || b2.ZMax() <= b1.ZMin())
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

/* Public Methods */

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

std::map<int, PrepRegion> InitPrepositions(R3SceneNode* node, int meters_of_context) {
    std::map<int, PrepRegion> prep_region_map;

    int resolution = 50;

    // Create a grid that encodes information about the points
    R3Grid grid = R3Grid(resolution, resolution, resolution);
   
    DrawingValues values = CreateDrawingValues(node, resolution);
    R3Affine world_to_grid_xform = PrepareWorldToGridXfrom(node->Centroid(), values);
  
    R3Box bb = node->BBox();
    bb.Transform(world_to_grid_xform);

    for (int prep_int = 0; prep_int < NUM_PREPOSITIONS; ++prep_int) {
        R3Box region = CalcSide(bb, prep_int, meters_of_context);
        PrepRegion pr = { prep_names[prep_int], prep_int, region };
        prep_region_map[prep_int] = pr;
    }

    return prep_region_map;
}

void CalcPrepositions(R3SceneNode* pri_obj, R3SceneNode* ref_obj, std::string pri_cat, 
        std::string ref_cat, Id2CatMap* id2cat, PrepRegionMap prep_region_map, 
        PrepMap* prep_map, int meters_of_context, FrequencyStats& freq_stats) {
    R3Box ref_bb = ref_obj->BBox();

    // Create and Apply Transform to put nearby region
    DrawingValues values = CreateDrawingValues(pri_obj, 50);
    R3Vector dist3d = (ref_obj->Centroid() - pri_obj->Centroid());
    values.dist3D = &dist3d;  // TODO: fix 
    R3Affine world_to_grid_xform = PrepareWorldToGridXfrom(ref_obj->Centroid(), values);
    ref_bb.Transform(world_to_grid_xform);

    if (dist3d.Length() > meters_of_context)
        return;

    PairMap* pair_count = freq_stats.pair_count; 
    ++(*pair_count)[pri_cat][ref_cat]; 
    freq_stats.pair_count = pair_count;

    for (int i = 0; i < prep_region_map.size(); ++i) {
        PrepRegion pr = prep_region_map[i];
        R3Box region = pr.region;

        R3Box* result = CalcIntersection(region, ref_bb);
        if (result == NULL)
            continue;
        
        float volume_of_overlap = result->Volume() / ref_bb.Volume();
        
        //if (volume_of_overlap < 0.5)
        //    continue;

        PrepositionStats prep_stats = (*prep_map)[pri_cat][ref_cat];
        UpdateStats(prep_stats, i);
        (*prep_map)[pri_cat][ref_cat] = prep_stats;
       
    }
}

int WritePrepositions(PrepMap* prepmap, FrequencyStats freq_stats) {
    PrepMap prep_map = *prepmap;
    for (auto it : prep_map) {
        std::string pri_cat = it.first;
        std::map<std::string, PrepositionStats> map = it.second;

        fprintf(stdout, "[%s]\n", pri_cat.c_str());
        for (auto it2 : map) {
            std::string ref_cat = it2.first;
            PrepositionStats prep_stats = it2.second;

            if (prep_stats.frontside == 0 && prep_stats.backside == 0 && prep_stats.above == 0 && 
                prep_stats.below == 0 && prep_stats.leftside == 0 && prep_stats.rightside == 0)
                continue;

            int div = ((*freq_stats.pair_count)[pri_cat][ref_cat]);

            fprintf(stdout, "\t[%s] frontside: %d backside: %d above: %d below: %d leftside: %d rightside: %d div: %d\n", 
                ref_cat.c_str(),
                prep_stats.frontside, // div, 
                prep_stats.backside, // div, 
                prep_stats.above, // div, 
                prep_stats.below, // div, 
                prep_stats.leftside, // div, 
                prep_stats.rightside,
                div); // div);

        }
    }

    return 1;
}
