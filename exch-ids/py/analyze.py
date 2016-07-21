import sys
import itertools
from random import randint
import csv

from maps import Id2Cat
from maps import Cat2Ids
import preprocess
import wordnet

# Command-Line Args
category        = sys.argv[1]
input_filename  = sys.argv[2]
output_filename = sys.argv[3]

##
## Constants
##

id_to_cat_filename = "data/object_names.csv"

id2cat = Id2Cat(id_to_cat_filename)
cat2ids = Cat2Ids(id_to_cat_filename)

categories = cat2ids.keys()
ids = id2cat.keys()

relationships = preprocess.rels.keys()

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
## Exchanagble Ids
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
    all_ids = cat2ids[category] # All cat2ids in the category

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
## Printing Relationships
##

def create_header():
    header = []
    header.append("id")
    header.append("num_relationships")
    for cat in categories:
        for rel in relationships:
            header.append(rel + "_" + cat)
    for location in wordnet.locations.keys():
        header.append(location)

    return header

def get_value(id, cat, rel, rel_log):
    if rel not in rel_log[id]:
        return 0
    elif cat not in rel_log[id][rel]:
        return 0
    else:
        return rel_log[id][rel][cat]


def print_rel_log(rel_log, counter):
    # For every id, print relationships with each category as well as # scenes
    # in common (necessitates a rewrite of rel_log)

    f = open('relationships.csv', 'w')
    try:
        writer = csv.writer(f, quoting=csv.QUOTE_NONNUMERIC)
        
        # Write Header
        header = create_header()
        writer.writerow(header)

        # Write rows (one per) 
        for id in rel_log: # ids present in scenes
            row = []
            row.append(id)
            row.append(counter[id])

            for cat in categories:
                for rel in relationships:
                    row.append(get_value(id, cat, rel, rel_log))
            
            for location, set in wordnet.locations.items():
                num_relationships = 0
                for cat in categories:
                    if cat not in set:
                        continue
                    
                    for rel in relationships:
                        num_relationships += get_value(id, cat, rel, rel_log)
                row.append(num_relationships)

            writer.writerow(row)

    finally:
        f.close()

##
## Main
##

if __name__ == '__main__':
    print "Starting ..." 

    print "\t- Analyzing scenes..."
    rel_log, counter = preprocess.preprocess(category, input_filename, id2cat)
   
    print "\t- Printing relationship file..."
    print_rel_log(rel_log, counter)

    #print "\t- Calculating exchangable sets..."
    #exchangable_ids = get_exchangable_ids(rel_log)
    #print exchangable_ids

    # print get_exchangable_ids("sofa")
    print "Done."
