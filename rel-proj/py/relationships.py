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

##
## Relationships
##

def touching(r):
    if r.sqrt_closest_dd > 0.15:
        return False
    return True

def within_1m(r):
    if r.sqrt_closest_dd > 1.0:
        return False
    return True

def within_2m(r):
    if r.sqrt_closest_dd > 4.0:
        return False
    return True

def within_3m(r):
    if r.sqrt_closest_dd > 9.0:
        return False
    return True

def above(r):
    if "Floor" in r.ref_cat:
        return True
    if "Wall" in r.ref_cat:
        return False
    if "Ceiling" in r.ref_cat:
        return False

    # Ref must be at least some below and none above
    if r.bc.below_bbox_z == 0 and (r.bc.above_bbox_z > 0 and r.pc.above_projection_z > 0):
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
    if "Floor" in r.ref_cat:
        return True

    # Ref must be at least some above and none below
    if r.bc.above_bbox_z == 0 and (r.bc.below_bbox_z > 0 and r.pc.below_projection_z > 0):
        return False

    # Ref must have a higher support (and not on the ground)
    if r.cz < 0.01:
        return False

    # Must be somewhat within the surface's x and y
    if r.bc.within_bbox_x == 0 or r.bc.within_bbox_y == 0:
        return False

    return True

def faces(r):
    if below(r) or above(r):
        return False
    if not within_2m(r):
        return False
    if r.bc.below_bbox_y != 0 or r.bc.above_bbox_y == 0:
        return False
    return True

def faces_away(r):
    if below(r) or above(r):
        return False
    # Within 2-m neighborhood
    if not within_2m(r):
        return False
    if r.bc.above_bbox_y != 0 or r.bc.below_bbox_y == 0:
        return False
    return True

def supports(r):
    # Corner Case
    if "door" in r.ref_cat:
        return False
    if "Ceiling" in r.ref_cat:
        return False
    if "Wall" in r.ref_cat:
        return False

    if not below(r):
        return False
    if not touching(r):
        return False
    return True

def supported_by(r):
    # Only reports "supported by floor" 
    if "Floor" not in r.ref_obj:
        return False
    if r.cz > 0.1 or r.cz < -0.1: # Try to accurately find supported by floor
        return False
    return True

def hanging(r):
    if "Ceiling" in r.ref_cat and "chandelier" in r.pri_cat:
        print_record(r)

    # Corner Case
    if "door" in r.pri_cat:
        return False
    if r.cz >= 0.0:
        return False
    if "Ceiling" in r.ref_cat and r.sqrt_closest_dd < 0.30:
        if "Ceiling" in r.ref_cat and "chandelier" in r.pri_cat:
            print "\t...hanging!"
        return True

    if not touching(r):
        return False
    
    return True
