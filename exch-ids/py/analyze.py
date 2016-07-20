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
## Constants
##

id_to_cat_filename = "data/object_names.csv"
categories = Categories(id_to_cat_filename)
ids = Ids(id_to_cat_filename)

##
## Aux
##

def pairwise(iterable):
    """ s -> (s0,s1), (s1,s2), (s2,s2) ..."""
    new_iterable = []
    for i, item1 in enumerate(iterable):
        for item2 in itertools.islice(iterable, i+1, None):
            new_iterable.append((item1,item2))

    return new_iterable

##
## Analysis Functions
##

class Score(object):
    def __init__(self):
        self.matches = 0.0
        self.misses  = 0.0

    def is_empty(self):
        return self.misses + self.matches == 0

    def ratio(self):
        if self.is_empty():
            return 0.0

        return self.matches / (self.matches + self.misses)

def compute_compatibility_score(id_1, id_2, rel_log):
    rels_that_matter = ["supported_by", "supports"]
    rel_set1 = rel_log[id_1]
    rel_set2 = rel_log[id_2]

    ratios = {"best" : 0.0}
    for rel in rels_that_matter:
        score = Score()
        for cat, count in rel_set1[rel].items():
            if cat in rel_set2[rel]:
                score.matches += 1
            else:
                score.misses += 1
        for cat, count in rel_set2[rel].items():
            if cat not in rel_set1[rel]:
                score.misses += 1
       
        if score.ratio > ratios["best"]:
            ratios["best"] = score.ratio

    return ratios["best"] * 10

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

##
## Main
##

if __name__ == '__main__':
    print "Starting ..." 
    
    print "\t- Analyzing scenes..."
    rel_log = preprocess.preprocess(category, input_filename, categories)
    
    print "\t- Calculating exchangable sets..."
    exchangable_ids = get_exchangable_ids(rel_log)
    print exchangable_ids

    # print get_exchangable_ids("sofa")
    print "Done."
