from collections import Counter

import relationships

#
# Relationship Sets
#

all_rels = {
    "hanging": relationships.hanging,
    "above": relationships.above,
    "below": relationships.below,
    "touching": relationships.touching,
    "faces": relationships.faces,
    "faces_away": relationships.faces_away,
    "in_front_of": relationships.return_false,
    "behind": relationships.return_false,
    "supports": relationships.supports,
    "supported_by": relationships.supported_by,
    "within_1m": relationships.within_1m,
    "within_2m": relationships.within_2m,
    "within_3m": relationships.within_3m,
}

testing = {
    "faces": relationships.faces,
}

# Change to change rel sets
rels = all_rels

# Opposites that can only be calculated one way
analogs = {
    "above": "below",
    "below": "above",
    "faces": "in_front_of",
    "faces_away": "behind",
    "supports": "supported_by",
}

hanging = []

#
# Record Object
#


class PC(object):
    # pc is a histogram of how many points on object B lie below, within,
    # or above the surfaces of A (for each dimension x, y, and z)
    def __init__(self, pc):
        self.below_projection_x = int(pc[0])
        self.within_projection_x = int(pc[1])
        self.above_projection_x = int(pc[2])
        self.below_projection_y = int(pc[3])
        self.within_projection_y = int(pc[4])
        self.above_projection_y = int(pc[5])
        self.below_projection_z = int(pc[6])
        self.within_projection_z = int(pc[7])
        self.above_projection_z = int(pc[8])


class BC(object):
    # bc is a histogram of how many points on object B lie below, within,
    # or above the bounding box of A (for each dimension x, y, and z)
    def __init__(self, bc):
        self.below_bbox_x = int(bc[0])
        self.within_bbox_x = int(bc[1])
        self.above_bbox_x = int(bc[2])
        self.below_bbox_y = int(bc[3])
        self.within_bbox_y = int(bc[4])
        self.above_bbox_y = int(bc[5])
        self.below_bbox_z = int(bc[6])
        self.within_bbox_z = int(bc[7])
        self.above_bbox_z = int(bc[8])


class DDC(object):
    # ddc is a histogram of how many points on object B have distances to
    # the closest point on object A (where the ddc[0] represents 0-1cm,
    # 1-2cm, etc.)
    def __init__(self, ddc):
        self.ddc = ddc
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


class Record(object):
    def __init__(self, row, map):
        self.pri_obj = row[0]
        self.pri_id = extract_id(self.pri_obj)
        self.pri_cat = get_cat(row[0], map)

        self.ref_obj = row[1]
        self.ref_id = extract_id(self.ref_obj)
        self.ref_cat = get_cat(row[1], map)

        # point_area is a scaling factor that can be used to compute a number
        # of points to a surface area (by multiplying by point_area)
        self.point_area = float(row[2])

        # distance between closest points
        self.sqrt_closest_dd = float(row[3])

        # npoints is the total number of points from object B used to compute
        # the following histograms
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
        end_ddc = start_ddc + num_dd_bins
        self.ddc = DDC(row[start_ddc:end_ddc])

        # bc is a histogram of how many points on object B lie below, within,
        # or above the bounding box of A (for each dimension x, y, and z)
        num_bbox_regions = 9
        start_bc = end_ddc
        end_bc = start_bc + num_bbox_regions
        self.bc = BC(row[start_bc:end_bc])

        # pc is a histogram of how many points on object B lie below, within,
        # or above the surfaces of A (for each dimension x, y, and z)
        num_projection_regions = 9
        start_pc = end_bc
        end_pc = start_pc + num_projection_regions
        self.pc = PC(row[start_pc:end_pc])

    def make_legible(self):
        return self.pri_cat + "(" + self.pri_id + ")" + " : " + \
            self.ref_cat + "(" + self.ref_id + ")"

    def __str__(self):
        return self.pri_obj + " " + self.ref_obj + " " +  \
            str(self.point_area) + " " + str(self.sqrt_closest_dd) + " " + \
            str(self.npoints) + " " + str(self.cx) + " " + \
            str(self.cy) + " " + str(self.cz)

#
# Utils
#


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


def get_cat(name, map):
    if "Wall" in name:
        return "Wall"
    elif "Floor" in name:
        return "Floor"
    elif "Ceiling" in name:
        return "Ceiling"

    id = extract_id(name)

    # val = map[id]

    return map[id]


def process_row(row, filter, id2cat):
    pri_name = row[0]
    if "Wall" in pri_name or "Floor" in pri_name or "Ceiling" in pri_name:
        return None

    if filter is None:
        return Record(row, id2cat)

    id = extract_id(pri_name)
    category = id2cat[id]
    if filter != category:
        return None

    return Record(row, id2cat)

#
# Exch-Ids Mode (Many-Scene Aggregate)
#


def init_id(id, rel_log):
    if id not in rel_log:
        rel_log[id] = {}
        for rel in rels.keys():
            rel_log[id][rel] = {}


def update(dict, key):
    if key not in dict:
        dict[key] = 1
    else:
        dict[key] += 1


def exch_ids(category, input_file, id2cat):
    counter = Counter()

    rel_log = {}  # id-rel-cat
    with open(input_file, 'r') as fh:
        for row in fh:
            row = row.split()
            r = process_row(row, category, id2cat)
            if r is None:
                continue

            id = r.pri_id
            init_id(id, rel_log)

            for rel, func in rels.items():
                if not func(r):
                    continue

                if rel in analogs:
                    init_id(r.ref_id, rel_log)
                    update(rel_log[r.ref_id][analogs[rel]], r.pri_cat)

                update(rel_log[id][rel], r.ref_cat)

            counter.update({id: 1})

    # Send back
    return rel_log, counter

#
# Relview Mode (One-Scene)
#


def relview(input_file, id2cat):
    analog_cleanup = []

    log = {}  # obj-obj-rel
    with open(input_file, 'r') as fh:
        for row in fh:
            row = row.split()
            r = process_row(row, None, id2cat)
            if r is None:
                continue

            obj1 = r.pri_obj
            obj2 = r.ref_obj

            if obj1 not in log:
                log[obj1] = {}
            if obj2 not in log[obj1]:
                log[obj1][obj2] = {}

            for rel, func in rels.items():
                result = func(r)

                log[obj1][obj2][rel] = result
                if result:  # Handle special cases
                    if rel == "hanging":
                        hanging.append((obj1, obj2))
                    if rel in analogs:
                        if analogs[rel] in rels:
                            analog_cleanup.append((obj1, obj2, analogs[rel]))

    for obj1, obj2, rel in analog_cleanup:
        if "Wall" in obj2 or "Floor" in obj2 or "Ceiling" in obj2:
            continue

        try:
            log[obj2][obj1][rel] = True
        except:
            print("KeyError on " + obj2 + " - " + obj1)

    if "supported_by" in rels:
        for obj1, obj2 in hanging:
            for alt in log[obj1]:
                if log[obj1][alt]["supported_by"]:
                    log[obj1][obj2]["hanging"] = False
                    break

    return log

#
# Learn Category Mode (Many-Scene)
#


def preprocess_many_scenes(input_file, id2cat):
    analog_cleanup = []
    # [].append((obj1, obj2, cat2, rels...)
    log = {}  # Need dictionary for analogs...
    # log : obj1 - obj2 - (rels, cat, cat2)

    with open(input_file, 'r') as fh:
        for row in fh:
            row = row.split()
            r = process_row(row, None, id2cat)
            if r is None:
                continue

            obj1 = r.pri_obj
            obj2 = r.ref_obj

            if obj1 not in log:
                log[obj1] = {}
            if obj2 not in log[obj1]:
                log[obj1][obj2] = {}

            for rel, func in rels.items():
                result = func(r)

                log[obj1][obj2][rel] = result
                if result:
                    if rel == "hanging":
                        hanging.append((obj1, obj2))
                    if rel in analogs:
                        if analogs[rel] in rels:
                            analog_cleanup.append((obj1, obj2, analogs[rel]))

    for obj1, obj2, rel in analog_cleanup:
        if "Wall" in obj2 or "Floor" in obj2 or "Ceiling" in obj2:
            continue
        try:
            log[obj2][obj1][rel] = True
        except:
            print("\tKeyError on " + obj2 + " - " + obj1)  # Don't be afraid

    if "supported_by" in rels:
        for obj1, obj2 in hanging:
            for alt in log[obj1]:
                if log[obj1][alt]["supported_by"]:
                    log[obj1][obj2]["hanging"] = False
                    break

    return log

