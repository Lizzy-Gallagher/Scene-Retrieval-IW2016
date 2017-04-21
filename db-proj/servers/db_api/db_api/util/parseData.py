from db_api import hashes 

def to_level_key(hash, level_num):
    return hash + ' ' + str(level_num)
def from_level_key(lk):
    return lk.split(' ')
def to_room_key(hash, level_num,room_num):
    return hash + ' ' + str(level_num) + ' ' + str(room_num)
def from_room_key(rk):
    return rk.split(' ')

def remove_prefix(object_name):
    return object_name[object_name.index('#') + 1:]

def parseData(data):
    scene_results = {}
    level_results = {}
    room_results  = {}
    for row in data:
        hash = hashes[row[0]]
        level_num = str(row[1])
        room_num = str(row[2])
        count = row[3]
        if hash not in scene_results:
            scene_results[hash] = 0
        scene_results[hash] += count

        lkey = to_level_key(hash, level_num)
        if lkey not in level_results:
            level_results[lkey] = 0
        level_results[lkey] += count
    
        rkey = to_room_key(hash, level_num, room_num)
        if rkey not in room_results:
            room_results[rkey] = 0
        room_results[rkey] += count

    scene_return = []
    for sr, value in scene_results.items():
        scene_return.append({
            'scene_hash' : sr,
            'value'      : value
        })

    level_return = []
    for lr, value in level_results.items():
        hash, level_num = from_level_key(lr)
        level_return.append({
            'scene_hash' : hash,
            'level_num'  : level_num,
            'value'      : value
        
        })

    room_return = []
    for rr, value in room_results.items():
        hash, level_num, room_num = from_room_key(rr)
        room_return.append({
            'scene_hash' : hash,
            'level_num'  : level_num,
            'room_num'   : room_num,
            'value'      : value
        })
    
    return scene_return, level_return, room_return

def parse_data_vis(data):
    scene_results = {}
    level_results = {}
    room_results = {}
    for row in data:
        hash = hashes[row[0]]
        level_num = str(row[1])
        room_num = str(row[2])
        object_id = remove_prefix(row[3])

        if hash not in scene_results:
            scene_results[hash] = []
        scene_results[hash].append(object_id)
        
        lkey = to_level_key(hash, level_num)
        if lkey not in level_results:
            level_results[lkey] = []
        level_results[lkey].append(object_id)
    
        rkey = to_room_key(hash, level_num, room_num)
        if rkey not in room_results:
            room_results[rkey] = []
        room_results[rkey].append(object_id)
    

    scene_return = []
    level_return = []
    room_return  = []
    for sr, objects in scene_results.items():
        scene_return.append({
            'scene_hash' : sr,
            'value' : len(objects),
            'objects' : objects
        })

    level_return = []
    for lr, objects in level_results.items():
        hash, level_num = from_level_key(lr)
        level_return.append({
            'scene_hash' : hash,
            'level_num'  : level_num,
            'value'      : len(objects),
            'objects'    : objects
        })

    room_return = []
    for rr, objects in room_results.items():
        hash, level_num, room_num = from_room_key(rr)
        room_return.append({
            'scene_hash' : hash,
            'level_num'  : level_num,
            'room_num'   : room_num,
            'value'      : len(objects),
            'objects'    : objects
        })

    return scene_return, level_return, room_return


