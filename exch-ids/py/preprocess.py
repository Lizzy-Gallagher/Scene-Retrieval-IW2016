from collections import Counter

import relationships

##
## I/O
##

#input_file = "obj_stats.txt"
#input_file = "foo.txt"
map = None

def extract_id(name):
    idx = -1

    # Case stores__*__*
    if "stores" in name:
        idx = name.rfind("stores")
        # Case: s__* 
    elif "__" in name:
        idx = name.rfind("s__")
        # Case: Numeric
    else:
        idx = name.rfind("_") + 1

    if idx == -1:
        raise LookupError("Unable to parse obj_id from: " + name)

    id = name[idx:]
    return id

def on_top(r):
    surface_area = r.point_area * r.npoints

    # Amount above
    num_above = r.pc.above_projection_z
    percent_above = (num_above * r.point_area) / surface_area

    # Within Horizatonal
    percent_within_x = (num_above * r.pc.within_projection_x) / surface_area
    percent_within_y = (num_above * r.pc.within_projection_y) / surface_area

    # Closest Points
    distance = r.sqrt_closest_dd

    print r.pri_cat + ":" + r.ref_cat + " - " + str(num_above) + " - " + \
            str(percent_above) + " - " + str(distance)
class MINIMALPC(object):
    # pc is a histogram of how many points on object B lie below, within,
    # or above the surfaces of A (for each dimension x, y, and z)
    def __init__(self, pc):
        self.below_projection_z  = int(pc[0])
        self.within_projection_z = int(pc[1])
        self.above_projection_z  = int(pc[2])

class MINIMALBC(object):
    # bc is a histogram of how many points on object B lie below, within,
    # or above the bounding box of A (for each dimension x, y, and z)
    def __init__(self, bc):
        self.within_bbox_x = int(bc[0])
        self.below_bbox_y  = int(bc[1])      
        self.within_bbox_y = int(bc[2])
        self.above_bbox_y  = int(bc[3])      
        self.below_bbox_z  = int(bc[4])
        self.above_bbox_z  = int(bc[5])

class PC(object):
    # pc is a histogram of how many points on object B lie below, within,
    # or above the surfaces of A (for each dimension x, y, and z)
    def __init__(self, pc):
        self.below_projection_x  = int(pc[0])
        self.within_projection_x = int(pc[1])
        self.above_projection_x  = int(pc[2])
        self.below_projection_y  = int(pc[3])
        self.within_projection_y = int(pc[4])
        self.above_projection_y  = int(pc[5])
        self.below_projection_z  = int(pc[6])
        self.within_projection_z = int(pc[7])
        self.above_projection_z  = int(pc[8])

class BC(object):
    # bc is a histogram of how many points on object B lie below, within,
    # or above the bounding box of A (for each dimension x, y, and z)
    def __init__(self, bc):
        self.below_bbox_x  = int(bc[0])
        self.within_bbox_x = int(bc[1])
        self.above_bbox_x  = int(bc[2])      
        self.below_bbox_y  = int(bc[3])      
        self.within_bbox_y = int(bc[4])
        self.above_bbox_y  = int(bc[5])      
        self.below_bbox_z  = int(bc[6])
        self.within_bbox_z = int(bc[7])
        self.above_bbox_z  = int(bc[8])

class DDC(object):
    # ddc is a histogram of how many points on object B have distances to
    # the closest point on object A (where the ddc[0] represents 0-1cm,
    # ddc[1] represents 1-2cm, etc.)
    def __init__(self, ddc):
        self.bin0 = int(ddc[0])
        self.bin1 = int(ddc[1])
        self.bin2 = int(ddc[2])
        self.bin3 = int(ddc[3])
        self.bin4 = int(ddc[4])
        self.bin5 = int(ddc[5])
        self.bin6 = int(ddc[6])
        self.bin7 = int(ddc[7])
        self.bin8 = int(ddc[8])
        self.bin9 = int(ddc[9])

def get_cat(name, map):
    if "W" in name:
        return "Wall"
    elif "F" in name:
        return "Floor"
    elif "C" in name:
        return "Ceiling"

    id = extract_id(name) 

    return map[id]

class MinimalRecord(object):
    def __init__(self, row, map):
        print row 
        self.pri_id = row[0]
        self.pri_cat = get_cat(row[0], map)

        self.ref_id  = row[1]
        self.ref_cat = get_cat(row[1], map)

        # distance between closest points
        self.sqrt_closest_dd = float(row[2])

        self.cz = float(row[3])

        # bc is a histogram of how many points on object B lie below, within,
        # or above the bounding box of A (for each dimension x, y, and z)
        num_bbox_regions = 6
        start_bc = 4
        end_bc = start_bc + num_bbox_regions
        print row[start_bc:end_bc]
        self.bc  = MINIMALBC(row[start_bc:end_bc])

        # pc is a histogram of how many points on object B lie below, within,
        # or above the surfaces of A (for each dimension x, y, and z)
        num_projection_regions = 3
        start_pc = end_bc
        end_pc = start_pc + num_projection_regions
        print row[start_pc:end_pc]
        self.pc = MINIMALPC(row[start_pc:end_pc])

    def make_legible(self):
        return self.pri_cat + "(" + self.pri_id + ")" + " : " + self.ref_cat + "(" + self.ref_id + ")"

class Record(object):
    def __init__(self, row, map):
        self.pri_obj = row[0]
        self.pri_id  = extract_id(self.pri_obj)
        self.pri_cat = get_cat(row[0], map)

        self.ref_obj = row[1]
        self.ref_id  = extract_id(self.ref_obj)
        self.ref_cat = get_cat(row[1], map)

        # point_area is a scaling factor that can be used to compute a number
        # of points to a surface area (by multiplying by point_area)
        self.point_area = float(row[2])

        # distance between closest points
        self.sqrt_closest_dd = float(row[3])

        # npoints is the total number of points from object B used to compute the
        # following histograms
        self.npoints = int(row[4])

        # c is the offset of the origin of object B from the origin of object A
        # in x, y, and z respectively (note that the origin is not the
        # centroid, and is weird for all walls, floors, and ceilings)
        self.cx = float(row[5])
        self.cy = float(row[6])
        self.cz = float(row[7])

        # ddc is a histogram of how many points on object B have distances to
        # the closest point on object A (where the ddc[0] represents 0-1cm,
        # ddc[1] represents 1-2cm, etc.)
        num_dd_bins = 10
        start_ddc = 8
        end_ddc   = start_ddc + num_dd_bins
        self.ddc = DDC(row[start_ddc:end_ddc])

        # bc is a histogram of how many points on object B lie below, within,
        # or above the bounding box of A (for each dimension x, y, and z)
        num_bbox_regions = 9
        start_bc = end_ddc
        end_bc = start_bc + num_bbox_regions
        self.bc  = BC(row[start_bc:end_bc])

        # pc is a histogram of how many points on object B lie below, within,
        # or above the surfaces of A (for each dimension x, y, and z)
        num_projection_regions = 9
        start_pc = end_bc
        end_pc = start_pc + num_projection_regions
        self.pc = PC(row[start_pc:end_pc])

    def add(self, record):
        self.point_area += record.point_area
        self.sqrt_closest_dd += record.sqrt_closest_dd
        self.npoints += record.npoints
        self.cx += record.cx
        self.cy += record.cy
        self.cz += record.cz

    def divide(self, num):
        self.point_area /= num
        self.sqrt_closest_dd /= num
        self.npoints /= num
        self.cx /= num
        self.cy /= num
        self.cz /= num

    def make_legible(self):
        return self.pri_cat + "(" + self.pri_id + ")" + " : " + self.ref_cat + "(" + self.ref_id + ")"

    def __str__(self):
        return self.pri_obj + " " + self.ref_obj + " " +  \
                str(self.point_area) + " " + str(self.sqrt_closest_dd) + " " + \
                str(self.npoints) + " " + str(self.cx) + " " + str(self.cy) + " " + \
                str(self.cz)

def process_row(row, filter, categories):
    pri_name = row[0]
    if "W" in pri_name or "F" in pri_name or "C" in pri_name: 
        return None
    if "W" in row[1]:
        return None

    id = extract_id(pri_name)
    category = categories[id]

    if filter != category:
        return None

    record = MinimalRecord(row, categories)
    return record

def create_key(record):
    return record.pri_id + record.ref_id

def preprocess(category, input_file, categories):
    # Load data
    counter = Counter()

    rels = {"supports" : relationships.supports,
            "supported_by" : relationships.supported_by,
            "above" : relationships.above,
            "below" : relationships.below,
            "touching" : relationships.touching,
            "within_1m" : relationships.within_1m,
            "within_2m" : relationships.within_2m,
            "within_3m" : relationships.within_3m,
            "faces" : relationships.faces,
            "faces_away" : relationships.faces_away }
    lim_rels = {
        "supports" : relationships.supports,
        "supported_by" : relationships.supported_by,
        "within_1m" : relationships.within_1m,
        "within_2m" : relationships.within_2m,
        "within_3m" : relationships.within_3m,
    }

    active_rels = lim_rels # Change me to change rels

    rel_log = {}
    with open(input_file, 'r') as fh:
        for row in fh:
            row = row.split()
            record = process_row(row, category, categories)
            if record is not None:
                id = record.pri_id

                for rel, func in active_rels.items():
                    if func(record):
                        if id not in rel_log:
                            rel_log[id] = {}
                            for rel in active_rels.keys():
                                rel_log[id][rel] = {} 

                        if record.ref_cat not in rel_log[id][rel]:
                            rel_log[id][rel][record.ref_cat] = 1
                        else:
                            rel_log[id][rel][record.ref_cat] += 1

                key = create_key(record)
                counter.update({key : 1})

    # Send back
    return rel_log

    # Printing for examination
    for id, rel_set in sorted(rel_log.items()):
        print id
        for rel, cat_set in sorted(rel_set.items()):
            print "\t" + rel
            for cat, count in sorted(cat_set.items()):
                print "\t\t" + cat + " : " + str(count)

    #for r in rel_dict["supports"]:
    #    print r.make_legible() + " : " + str(r.sqrt_closest_dd)

    # Average values
    #for key, record in dict.items():
    #    record.divide(counter[key])

    # for every category of ref_objects, average all numeric values

    # use AWK to do this

