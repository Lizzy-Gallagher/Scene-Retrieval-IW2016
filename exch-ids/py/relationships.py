##
## Prototypes
##

def touching(r):
    if r.sqrt_closest_dd > 0.25:
        return False

    #""" NEED TO IMPROVE """
    #if r.ddc.bin0 == 0 and r.ddc.bin1 == 0 and r.ddc.bin2 == 0 and r.ddc.bin3 == 0:
    #    return False
    return True


##
## Current
##





##
## Completed
##

def below(r):
    total_in_column = r.pc.above_projection_z + r.pc.within_projection_z + r.pc.below_projection_z
    if total_in_column == 0:
        return False
    if r.pc.above_projection_z != 0 or r.bc.above_bbox_z != 0:
        return False

    return True

def above(r):
    total_in_column = r.pc.above_projection_z + r.pc.within_projection_z + r.pc.below_projection_z
    if total_in_column == 0:
        return False
    if r.pc.below_projection_z != 0 or r.bc.below_bbox_z != 0:
        return False
    
    return True

# TODO: Fix this
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

def supports(r):
    if "Floor" in r.ref_obj:  # TODO: Fix this
        return False

    if not above(r):
        return False
    if not touching(r):
        return False
    if r.cz <= 0.01 and r.bc.within_bbox_z == 0:
        return False

    #if r.pc.above_projection_z == 0:
    #    return False

    if r.bc.above_bbox_x >= r.bc.within_bbox_x or r.bc.below_bbox_x >= r.bc.within_bbox_x:
        return False
    if r.bc.above_bbox_y >= r.bc.within_bbox_y or r.bc.below_bbox_y >= r.bc.within_bbox_y:
        return False


    print r.make_legible()
    print "\tz: " + str(r.bc.above_bbox_z) + " " + str(r.bc.within_bbox_z) + " " +str(r.bc.below_bbox_z)
    print "\tz: " + str(r.pc.above_projection_z) + " " + str(r.pc.within_projection_z) + " " +str(r.pc.below_projection_z)

    print "\ty: " + str(r.bc.above_bbox_y) + " " + str(r.bc.within_bbox_y) + " " +str(r.bc.below_bbox_y)
    print "\ty: " + str(r.pc.above_projection_y) + " " + str(r.pc.within_projection_y) + " " +str(r.pc.below_projection_y)

    print "\tx: " + str(r.bc.above_bbox_x) + " " + str(r.bc.within_bbox_x) + " " +str(r.bc.below_bbox_x)
    print "\tx: " + str(r.pc.above_projection_x) + " " + str(r.pc.within_projection_x) + " " +str(r.pc.below_projection_x)

    print "\tcz: " + str(r.cz)
    print "\tnpoints: " + str(r.npoints)

    return True

def supported_by(r):
    if not below(r):
        return False
    if not touching(r):
        return False
    if r.cz >= 0 and r.pc.within_projection_z == 0:
        return False

    return True
