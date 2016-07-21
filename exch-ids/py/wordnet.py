# Wordnet implementation for exchangible ids


dining_area = [
    "chandelier",
    "dinning_table"
]

living_room = [
    "sofa",
    "coffee_table"
]

bed_room = [
    "bunker_bed",
    "single_bed",
    "double_bed",
    "baby_bed",
    "dressing_table",
    "dresser"
]

kitchen = [
    "hanging_kitchen_cabinet",
    "kitchen_cabinet",
    "kitchen_set",
    "kitchen_appliance",
    "kitchenware",
    "fridges"
]

bathroom = [
    "bathroom_stuff",
    "bathtub",
    "toilet",
    "shower",
    "wash_basins"
]

outdoor = [
    "outdoor_lamp",
    "pool",
    "outdoor_spring",
    "outdoor_rest",
    "mailbox",
    "fences_gate",
    "recreation,basketball_hoop",
]

office = [
    "computer",
    "workplace",
    "office_chairs",
    "desk"
]

utility = [
    "household_appliance,washer",
    "household_appliance,dryer"
]

locations = {
    "dining_area" : dining_area,
    "living_room" : living_room,
    "bed_room" : bed_room,
    "kitchen" : kitchen,
    "bathroom" : bathroom,
    "outdoor" : outdoor,
    "office" : office,
    "utility" : utility,
}

def get_location(cat):
    for location, set in locations.items():
        if cat in set:
            return location
    return None

#      UNUSED CATEGORIES
#  ------------------------
#  pillow
#  shoes
#  ottoman
#  chair
#  stair
#  fireplaces
#  table_lamp
#  safe
#  fan
#  partitions
#  recreation
#  bar
#  idk
#  household_appliance,vacuuming
#  people
#  music,piano
#  tvs
#  table_and_chair
#  category
#  paintings
#  pets
#  hangers
#  tv_bench
#  missing
#  rug
#  wall_lamp
#  bench_chair
#  shelves
#  column
#  heater
#  armchair
#  toys
#  music
#  curtain
#  door
#  recreation,pool_table
#  gym
#  candel
#  cloth
#  bookshelf
#  closets_wardrobes_cabinets
#  plants
#  shoes_cabinet
#  car
#  household_appliance
#  trash_can
#  whitebroad
#  ac
#  household_appliance,ironing
#  figurines
#  floor_lamps
#  tripole
#  stand
#  vases
#  arch
#  windows
#  clock
#  switch
#  
