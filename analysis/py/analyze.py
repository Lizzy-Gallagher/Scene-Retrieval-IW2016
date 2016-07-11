import csv
import sys

# Command-Line Args
input_filename = sys.argv[1]
output_filename = sys.argv[2]
category_id = sys.argv[3]

##
## CONSTANTS
##

id_to_cat_filename = "data/object_names.csv"

distances = [0.5, 1.0, 1.5, 2.0, 2.5,
             3.0, 3.5, 4.0, 4.5, 5.0]

axes = ['x','y','z']

directions = ['pos_x', 'neg_x',
              'pos_y', 'neg_y',
              'pos_z', 'neg_z']

strictness = ['high', 'med', 'low']

class Constants(objects):
    def __init__(self):
        # Three thresholds for match strictness:
        #   - Strict: ALL binary relationships must be equal
        #   - Medium: ALL orthogonal directions must have a binary relationship in
        #             common
        #   - Leniant: HALF of orthogonal directions "" AND/OR "Near" 
        self.match_strictness = 'med'

        # Three thresholds for category strictness: 
        #   - Strict: 100% categories must "match"
        #   - Medium: 60% of categories must "match" 
        #   - Leniant: 40% of cateogries must "match"
        self.category_strictness = 'low'

##
## RELATIONSHIP CLASSES
##

class Near(object):
    def __init__(self):
        self.dist = [false] * size(direction)

class OrthogonalRelationship(object):
    """ Stores x/y/z values """
    def __init__(self, direction):
        self.direction = direction

        # Off-Axis relationships
        self.within_column    = false
        self.partially_column = false
        self.in_direction     = false

        # On-Axis relationships
        self.dist_to_axis = 0.0

class Relationships(object):
    """ Stores all realtionships """
    def __init__(self, id):
        self.id = id

        self.x = [OrthogonalRelationship('pos_x'), OrthogonalRelationship('neg_x')]
        self.y = [OrthogonalRelationship('pos_y'), OrthogonalRelationship('neg_y')]
        self.z = [OrthogonalRelationship('pos_z'), OrthogonalRelationship('neg_z')]
        
        self.near = Near()

##
## I/O
##

def read_results():
    """ Read CSV file, create and update relationships """
    with open(input_filename) as csvfile:
        reader = csv.reader(csvfile, delimiter=',')
        for row in reader:
            print (', '.join(row))

def write_stats():
    """ Write statistics """
    with open(output_filename, 'w+') as outputfile:
        writer = csv.writer(outputfile, delimiter=' ')
        writer.writerow(['a', 'b', 'c'])

def get_category_from_id(id):
    with open(id_to_cat_filename) as id_to_cat:
        reader = csv.reader(id_to_cat, delimiter=',')
        for row in reader:
            my_dict[row[0]] = row[2]

##
## Analysis Functions
##

def matches(id1, id2):
    constant = Constants()
    strictness = constant.match_strictness

    if strictness == 'high':
        for axis in axes:
            for dir in directions:
               return false 

    elif strictness == 'med':
        for axis in axes:
            for dir in directions:
                return false

    elif strictness == 'low':
        if near:
            return true
        for axis in axes:
            for dir in directions:
                return false

    print "Unimplemented"

def compute_compatibility_score(id_1, id_2):

    matches = 0

    # for all ref_cat
    #   if matches(id1, id2):
    #       matches += 1
    # if matches / total_num > threshold
    #   return true

    print "Unimplemented"

def get_similar_objects_by_category(category):
    # Create a dictionary of the following form:
    # pri_id -> [ref_id2 -> relationships]

    # This involves calculating the relationships
    #   Simple: 
    #       - contained in columns of 6-orthogonal directions (6) 
    #       - partially in columns of 6-orthogonal directions (6)
    #       - in 6-orthogonal directions, but outside column  (6)
    #       - "Near" to within 0.5m - 5m increments           (10)

    # For each id in dict compare the fraction of shared relationships
    # of the 28-relationships with each of the other categories

    print "Unimplemented"

#def get_similar_objects_by_id(id):
#    print "Unimplemented"

##
## MAIN METHOD
##

if __name__ == '__main__':
    get_similar_objects_by_category(category)
    
