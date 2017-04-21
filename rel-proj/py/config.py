# This is a file to collect the constants of the program

import relationships

relationships_set = {
    "hanging"   : relationships.hanging,
    "above" : relationships.above,
    "below" : relationships.below,
    "touching" : relationships.touching,
    "faces" : relationships.faces,
    "faces_away" : relationships.faces_away,
    "in_front_of" : relationships.return_false,
    "behind" : relationships.return_false,
    "supports" : relationships.supports,
    "supported_by" : relationships.supported_by,
    "within_1m" : relationships.within_1m,
    "within_2m" : relationships.within_2m,
    "within_3m" : relationships.within_3m,
}

analogs = {
    "above": "below",
    "below": "above",
    "faces": "in_front_of",
    "faces_away": "behind",
    "supports": "supported_by",
}



