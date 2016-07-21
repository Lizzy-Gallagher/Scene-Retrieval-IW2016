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
## Current
##

# TODO: Fix this, probably just a distance thing...
def next_to(r): # x-value
    if below(r) or above(r):
        return False
    
    # Left-side
    total_in_column = r.pc.above_projection_x + r.pc.within_projection_x
    if total_in_column == 0:
        return False
    
    if r.pc.within_bbox_y == 0:
        return False

    return False

def against(r):
    return False



def lays_on(r):
    return False

def between(r):
    return False

def behind(r):
    # analog "faces away"
    return False

def in_front_of(r):
    # analog "faces"
    return False

def return_false(r):
    return False

##
## Completed
##

def touching(r):
    if r.sqrt_closest_dd > 0.25:
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

def below(r):
    # In the Z-Column
    total_in_column = r.pc.above_projection_z + r.pc.within_projection_z + r.pc.below_projection_z
    if total_in_column == 0:
        return False
    if r.pc.above_projection_z != 0 or r.bc.above_bbox_z != 0:
        return False

    # If floor is ref, must be on
    if "Floor" in r.ref_cat and r.cz > 0.0:
        return False 

    # Not on floor, support is lower
    if "Floor" not in r.ref_cat and r.cz >= -.01:
        return False

    return True

def above(r):
    # In the Z-Column
    total_in_column = r.pc.above_projection_z + r.pc.within_projection_z + r.pc.below_projection_z
    if total_in_column == 0:
        return False
    if r.pc.below_projection_z != 0 or r.bc.below_bbox_z != 0:
        return False

    # Not on floor, support is higher
    if r.cz <= 0.01:
        return False

    return True

def faces_away(r):
    if "Floor" in r.ref_cat:
        return False
    if below(r) or above(r):
        return False
    if not within_1m(r):
        return False
    if r.bc.above_bbox_y != 0 or r.bc.below_bbox_y == 0:
        return False
    return True

def faces(r):
    if "Floor" in r.ref_cat:
        return False
    if below(r) or above(r):
        return False
    if not within_1m(r):
        return False
    if r.bc.below_bbox_y != 0 or r.bc.above_bbox_y == 0:
        return False
    return True

def supports(r):
    if "Floor" in r.ref_cat:
        return False
    if not above(r):
        return False
    if not touching(r):
        return False
    if r.bc.within_bbox_x == 0 or r.bc.within_bbox_y == 0:
        return False
    return True

def supported_by(r):
    if not below(r):
        return False
    if not touching(r):
        return False
    return True

def hanging(r):
    if "W" not in r.ref_cat:
        return False
    if not touching(r):
        return False
    if r.cz == 0.0:
        return False
    return True

def stands_on(r):
    if not supported_by(r):
        return False

    npoints = float(r.npoints)
    print r.make_legible()
    #lower = 0
    #mid   = 0
    #upper = 0
    for i, val in enumerate(r.ddc.ddc):
        print str(i) + " " + val
        #if i < 5:
        #    lower += float(val)
        #elif i < 7:
        #    mid += float(val)
        #else:
        #    upper += float(val)
    #print lower / npoints
    #print mid   / npoints
    #print upper / npoints
    #print "d: " + str(r.sqrt_closest_dd)
    #print 


    return False
