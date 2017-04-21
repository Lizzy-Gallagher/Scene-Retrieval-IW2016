# Maps module

import csv

class Obj2Data(dict):
    """ Object Name -> (levelnum roomnum objectnum, modelid)"""
    def __init__(self, filename):
        if filename is None:
            return

        level_idx = 1
        room_idx = 2
        object_idx = 3
        model_id_idx = 4
        name_idx = 5
        with open(filename) as object_file:
            reader = csv.reader(object_file, delimiter=',')
            for row in reader:
                self[row[name_idx]] = (row[level_idx], row[room_idx],
                                       row[object_idx], row[model_id_idx])

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
