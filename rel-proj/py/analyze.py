import sys
import itertools
from random import randint
import csv
import os.path

from maps import Id2Cat
from maps import Cat2Ids
import preprocess
import wordnet

import relationships

##
## Command-Line Argument Parsing
##

import argparse
parser = argparse.ArgumentParser()
parser.add_argument("input_filename", help="the input filename")
parser.add_argument("output_filename", help="the output filename")
parser.add_argument("-category", action="store", dest="category",
                    help="the category to filter by")
parser.add_argument("-mode",  action="store", dest="mode",
                    help="Set mode")
args = parser.parse_args()


# Command-Line Args
category        = args.category
input_filename  = args.input_filename
output_filename = args.output_filename

weka_mode = ".arff" in output_filename 

class Mode(object):
    isExchIds = False
    isRelView = False
    isLearnCategory = False

    def __init__(self, mode):
        if mode == None:
            self.isExchIds = True
        elif mode == "exch":
            self.isExchIds = True
        elif mode == "relview":
            self.isRelView = True
        elif mode == "mlcat":
            self.isLearnCategory = True
        else:
            raise ValueError("Unexpected Mode: " + mode)

mode = Mode(args.mode)
to_include = ["Ceiling", "Floor", "dinning_table", "sofa", "chair"]

##
## Constants
##

header_filename = '../data/weka/header'

id_to_cat_filename = "../data/object_names.csv"
id2cat = Id2Cat(id_to_cat_filename)
ids = id2cat.keys()

cat2ids = Cat2Ids(id_to_cat_filename)
categories = cat2ids.keys()
categories.append("Wall")
categories.append("Floor")
categories.append("Ceiling")

cat2num = {}
for i, category in enumerate(set(categories)):
    cat2num[category] = i

rels = preprocess.rels.keys()

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
## Printing
##

def create_header():
    header = []
    if mode.isRelView:
        header.append("Obj1")
        header.append("Obj2")
        for rel in sorted(rels):
            header.append(rel)
        return header

    elif mode.isExchIds:
        header.append("id")
        header.append("num_relationships")
        for cat in categories:
            for rel in rels:
                header.append(rel + "_" + cat)
        for location in wordnet.locations.keys():
            header.append(location)
    
    elif mode.isLearnCategory:
        header.append("id1")
        header.append("cat1")
        header.append("cat2")
        for rel in sorted(rels):
            header.append(rel)

    return header

def get_value(id, cat, rel, rel_log):
    if rel not in rel_log[id]:
        return 0
    elif cat not in rel_log[id][rel]:
        return 0
    else:
        return rel_log[id][rel][cat]

def print_exch_ids(rel_log, counter):
    # For every id, print rels with each category as well as # scenes
    # in common (necessitates a rewrite of rel_log)

    f = open(output_filename, 'w')
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
                for rel in rels:
                    row.append(get_value(id, cat, rel, rel_log))
            
            for location, set in wordnet.locations.items():
                num_relationships = 0
                for cat in categories:
                    if cat not in set:
                        continue
                    
                    for rel in rels:
                        num_relationships += get_value(id, cat, rel, rel_log)
                row.append(num_relationships)

            writer.writerow(row)

    finally:
        f.close()

def print_relview(relview_log):
    # For every object # print the rels with other objects
    # 0 is false, 1 is true


    f = open(output_filename, 'w')
    try:
        writer = csv.writer(f, quoting=csv.QUOTE_NONNUMERIC)

        # Header
        header = create_header()
        writer.writerow(header)

        # Write rows:
        for obj1 in relview_log:
            for obj2, rels in relview_log[obj1].items():
                row = []
                row.append(obj1)
                row.append(obj2)
                for rel in sorted(rels):
                    val = rels[rel]
                    if val:
                        row.append(1)
                    else:
                        row.append(0)

                writer.writerow(row)
    finally:
        f.close()


def create_weka_header():
    cat1_lst = "{"
    for cat1 in categories:
        cat1_lst += str(cat1) + ","
    cat1_lst = cat1_lst[:-1] + "}"

    rows = []
    rows.append("@RELATION Relationships")
    rows.append("\n")

    for cat2 in sorted(to_include):
        for rel in sorted(rels):
            rows.append("@ATTRIBUTE " + cat2 + "-" + rel + " NUMERIC")
    rows.append("@ATTRIBUTE cat1 " + cat1_lst)

    rows.append("\n")
    return rows

def print_learn_category(log, weka_compatible=False):
    # Print header file
    if weka_compatible:
        # Check for existence of header
        if not os.path.isfile(header_filename):
            # If it does not exist, create it
            f = open(header_filename, 'w')
            try:
                header = create_weka_header()
                for row in header:
                    f.write(row + "\n")
                f.write("@DATA\n")
            finally:
                f.close()
            
    f = open(output_filename, 'w')
    try:
        writer = csv.writer(f, quoting=csv.QUOTE_NONE)
        
        # Header
        if not weka_compatible:
            header = create_header()
            writer.writerow(header)
        
        for obj1 in log:
            row = []
            # obj1 - ref_cat - rels
            rels_by_cat = {}

            for obj2 in log[obj1]:
                rels = log[obj1][obj2]

                ref_cat = preprocess.get_cat(obj2, id2cat)
                if ref_cat not in rels_by_cat:
                    rels_by_cat[ref_cat] = {}
                for rel in sorted(rels):
                    if rel not in rels_by_cat[ref_cat]:
                        rels_by_cat[ref_cat][rel] = 0
                    if rels[rel]:
                        rels_by_cat[ref_cat][rel] += 1

            # Append to row in order
            for ref_cat in sorted(to_include):
                for rel in sorted(rels):
                    if ref_cat in rels_by_cat:
                        row.append(rels_by_cat[ref_cat][rel])
                    else:
                        row.append(0)

            row.append(preprocess.get_cat(obj1, id2cat))
            writer.writerow(row)
    
    finally:
        f.close()
 

##
## Main
##

if __name__ == '__main__':
    print "Starting ..." 

    if mode.isExchIds:
        print "\t- Analyzing scenes..."
        rel, counter = preprocess.exch_ids(category, input_filename, id2cat)
   
        print "\t- Printing relationship file..."
        print_exch_ids(rel, counter)
        
        #print "\t- Calculating exchangable sets..."
        #exchangable_ids = get_exchangable_ids(rel_log)
        #print exchangable_ids
        # print get_exchangable_ids("sofa")
    
    elif mode.isRelView:
        print "\t- Preprocessing data..."
        log = preprocess.relview(input_filename, id2cat)

        print "\t- Printing relationship file..."
        print_relview(log)
    
    elif mode.isLearnCategory:
        print "\t- Preprocessing data..."
        log = preprocess.learn_category(input_filename, id2cat)

        print "\t- Printing category learning file..."
        print_learn_category(log, weka_mode)


    print "Done."
