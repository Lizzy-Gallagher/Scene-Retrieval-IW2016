import sys
import itertools
from random import randint

from maps import Categories
from maps import Ids
import preprocess

# Command-Line Args
category        = sys.argv[1]
input_filename  = sys.argv[2]
output_filename = sys.argv[3]

##
## CONSTANTS
##

id_to_cat_filename = "data/object_names.csv"
categories = Categories(id_to_cat_filename)
ids = Ids(id_to_cat_filename)

distances = [0.5, 1.0, 1.5, 2.0, 2.5,
             3.0, 3.5, 4.0, 4.5, 5.0]

axes = ['x','y','z']

directions = ['pos_x', 'neg_x',
              'pos_y', 'neg_y',
              'pos_z', 'neg_z']

strictness = ['high', 'med', 'low']

class Constants(object):
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
## Analysis Functions
##

def matches(id1, id2):
    """ Returns bool of matching """
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

def compute_compatibility_score(id_1, id_2, rel_log):

    matches = 0.0
    misses = 0.0
    rel_set1 = rel_log[id_1]
    rel_set2 = rel_log[id_2]

    rels_that_matter = ["supported_by"]
    for rel in rels_that_matter:
        for cat, count in rel_set1[rel].items():
            if cat in rel_set2[rel]:
                matches += 1
            else:
                misses += 1
        for cat, count in rel_set2[rel].items():
            if cat not in rel_set1[rel]:
                misses += 1

    if misses + matches == 0:
        return 0

    ratio = matches / (matches + misses)
    return ratio * 10

def pairwise(iterable):
    """ s -> (s0,s1), (s1,s2), (s2,s2) ..."""
    new_iterable = []
    for i, item1 in enumerate(iterable):
        for item2 in itertools.islice(iterable, i+1, None):
            new_iterable.append((item1,item2))

    return new_iterable

def get_score(record):
    """ return record.score """
    return record[2]

def get_exchangable_ids(rel_log):
    all_ids = ids[category] # All ids in the category

    if len(all_ids) <= 1:
        return None
 
    records = []
    for id1, id2 in pairwise(all_ids):
        if id1 not in rel_log or id2 not in rel_log:
            continue

        score = compute_compatibility_score(id1, id2, rel_log)
        records.append((id1, id2, score))
        print "\t\t- (" + str(id1) + ")(" + str(id2) + ") : " + str(score)

    threshold = 1 # score necessary for compatibility

    # Only inlclude records above a certain threshold
    records = filter(lambda record: get_score(record) > threshold,
                               records)
    # Sort for human readability
    records = sorted(records, reverse=True)
    
    # Exchangable sets is a connected component
    exchangable_sets = []
    setted_ids = []
    for record in records:
        id1 = record[0]
        id2 = record[1]

        if id1 not in setted_ids:
            setted_ids.append(id1)
            
            if id2 not in setted_ids: 
                setted_ids.append(id2)
                exchangable_sets.append([id1, id2])

            else:
                for set in exchangable_sets:
                    if id2 in set:
                        set.append(id1)
                        break

        elif id2 not in setted_ids:
            setted_ids.append(id2)

            for set in exchangable_sets:
                if id1 in set:
                    set.append(id2)
                    break

    return exchangable_sets

if __name__ == '__main__':
    print "Starting ..." 
    
    print "\t- Analyzing scenes..."
    rel_log = preprocess.preprocess(category, input_filename, categories)
    
    print "\t- Calculating exchangable sets..."
    exchangable_ids = get_exchangable_ids(rel_log)
    print exchangable_ids

    # print get_exchangable_ids("sofa")
    print "Done."
