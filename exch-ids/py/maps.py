# Maps module

import csv

class Id2Cat(dict):
    """ Object Id --> Category """
    def __init__(self, filename):
        with open(filename) as cat_file:
            reader = csv.reader(cat_file, delimiter=',')
            for row in reader:
                self[row[0]] = row[1]

class Cat2Ids(dict):
    """ Category --> Object Ids """
    def __init__(self, filename):
        with open(filename) as cat_file:
            reader = csv.reader(cat_file, delimiter=',')
            for row in reader:
                id = row[0]
                cat = row[1]
                
                if cat not in self:
                    self[cat] = []
                
                self[cat].append(id)
