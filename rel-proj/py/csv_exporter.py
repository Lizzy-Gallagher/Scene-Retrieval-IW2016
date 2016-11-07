
# call to export the current relationships being used (perhaps in a
# constants.py).

import config
import csv

from maps import Cat2Ids

relationships_filename = "relationships.csv"
categories_filename = "categories.csv"

def create_relationships_csv():
    f = open(relationships_filename, 'w')
    try:
        writer = csv.writer(f, quoting=csv.QUOTE_MINIMAL)

        # Write header
        header = ["id", "name"]
        writer.writerow(header)
        

        for i, k in enumerate(sorted(config.total_rels.keys())):
            row = [i+1, k]
            writer.writerow(row)
            
    finally:
        f.close()

def create_categories_csv():
    # Perhaps break out into own method
    id2cat = Cat2Ids("../data/object_names.csv")

    f = open(categories_filename, 'w')
    try:
        writer = csv.writer(f, quoting=csv.QUOTE_MINIMAL)

        # Write header
        header = ["id", "name", "wordnet_id"]
        writer.writerow(header)

        for i, cat in enumerate(id2cat.keys()):
            row = [i, cat, 0]
            writer.writerow(row)

    finally:
        f.close()

create_categories_csv()
create_relationships_csv()
