from collections import Counter


def print_record(r):
    print r.make_legible()
    print "\tz (bb): " + str(r.bc.above_bbox_z) + " " + str(r.bc.within_bbox_z) + " " +str(r.bc.below_bbox_z)
    print "\tz (p): " + str(r.pc.above_projection_z) + " " + str(r.pc.within_projection_z) + " " +str(r.pc.below_projection_z)

    print "\ty (bb): " + str(r.bc.above_bbox_y) + " " + str(r.bc.within_bbox_y) + " " +str(r.bc.below_bbox_y)
    print "\ty  (p): " + str(r.pc.above_projection_y) + " " + str(r.pc.within_projection_y) + " " +str(r.pc.below_projection_y)

    print "\tx (bb): " + str(r.bc.above_bbox_x) + " " + str(r.bc.within_bbox_x) + " " +str(r.bc.below_bbox_x)
    print "\tx  (p): " + str(r.pc.above_projection_x) + " " + str(r.pc.within_projection_x) + " " +str(r.pc.below_projection_x)

    print "\tcz: " + str(r.cz)
    print "\tnpoints: " + str(r.npoints)
    print "\tdist: " + str(r.sqrt_closest_dd)


##
## Placeholder
##

def return_false(r):
    return False

##
## Utils
##

def is_floor_or_ceiling(r):
    if "Floor" == r.ref_cat or "Ceiling" == r.ref_cat:
        return True
    return False

def is_wall(r):
    return "Wall" in r.ref_obj

def is_obj(r):
    #if "Floor" in r.ref_cat or "Ceiling" in r.ref_cat: #or "Wall" in r.ref_cat:
    #    return False
    return True

def correct_wall_mode(r, wall_mode):
    if wall_mode:
        if "Wall" not in r.ref_cat:
            return False
    else: 
        if "Wall" in r.ref_cat:
            return False
    return True

def correct_floor_mode(r, floor_mode):
    if floor_mode:
        if "Floor" not in r.ref_cat:
            return False
    else: 
        if "Floor" in r.ref_cat:
            return False
    return True

def in_z_column(r):
    # No longer works...
    total_in_column = r.pc.above_projection_z + r.pc.within_projection_z + r.pc.below_projection_z
    if total_in_column == 0:
        return False
    return True

##
## Relationships
##

def touching(r, wall_mode=False, floor_mode=False):
    if not correct_wall_mode(r, wall_mode):
        return False
    if not correct_floor_mode(r, floor_mode):
        return False
    #if "Ceiling" in r.ref_cat:
    #    return False
    if r.sqrt_closest_dd > 0.15:
        return False
    return True

def touching_wall(r):
    return touching(r, True)

def within_1m(r):
    if not is_obj(r): 
        return False
    if r.sqrt_closest_dd > 1.0:
        return False
    return True

def within_2m(r):
    if not is_obj(r):
        return False
    if r.sqrt_closest_dd > 4.0:
        return False
    return True

def within_3m(r):
    if not is_obj(r):
        return False
    if r.sqrt_closest_dd > 9.0:
        return False
    return True

def above(r):
    if "Floor" in r.ref_cat:
        return True
    if "Wall" in r.ref_cat:
        return False

    if not in_z_column(r):
        return False
    
    # Ref must be at least some below and none above
    if r.bc.below_bbox_z == 0 or (r.bc.above_bbox_z > 0 and r.pc.above_projection_z > 0):
        return False

    # Ref must have a lower support (and not on the ground)
    if r.cz > -0.01:
        return False
    
    # Must be contained within the surface's x and y
    if r.bc.within_bbox_x == 0 or r.bc.within_bbox_y == 0:
        return False

    return True

def below(r):
    if "Ceiling" in r.ref_cat:
        return True
    if "Wall" in r.ref_cat:
        return False

    if not in_z_column(r):
        return False

    # Ref must be at leaset some above and none below
    if r.bc.above_bbox_z == 0 or (r.bc.below_bbox_z > 0 and r.pc.below_projection_z > 0):
        return False

    # Ref must have a higher support (and not on the ground)
    if r.cz < 0.01:
        return False

    # Must be somewhat within the surface's x and y
    if r.bc.within_bbox_x == 0 or r.bc.within_bbox_y == 0:
        return False

    return True

def faces(r, wall_mode=False):
    if not correct_wall_mode(r, wall_mode):
        return False
    if is_floor_or_ceiling(r):
        return False
    if not is_wall(r) and (below(r) or above(r)):
        return False
    if not is_wall(r) and not within_1m(r):
        return False
    if r.bc.below_bbox_y != 0 or r.bc.above_bbox_y == 0:
        return False
    return True

def faces_wall(r):
    return faces(r, True);

def faces_away(r, wall_mode=False):
    if not correct_wall_mode(r, wall_mode):
        return False
    if is_floor_or_ceiling(r):
        return False
    if not is_wall(r) and (below(r) or above(r)):
        return False
    if not is_wall(r) and not within_1m(r):
        return False
    if r.bc.above_bbox_y != 0 or r.bc.below_bbox_y == 0:
        return False
    return True

def faces_away_wall(r):
    return faces_away(r, True)

def supports(r):
    if not is_obj(r):
        return False
    if "door" in r.ref_cat:
        return False
    if not above(r):
        return False
    if not touching(r):
        return False
    if r.bc.within_bbox_x == 0 or r.bc.within_bbox_y == 0:
        return False
    
    return True

def supported_by(r):
    if "Floor" not in r.ref_obj:
        return False
    if not touching(r, floor_mode=True):
        return False
    return True

def hanging(r, wall_mode=False):
    if not correct_wall_mode(r, wall_mode):
        return False
    if "door" in r.pri_cat:
        return False
    if not touching(r, wall_mode=True):
        return False
    if r.cz >= 0.0:
        return False
    return True

def hanging_wall(r):
    return hanging(r, True)


