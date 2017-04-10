from db_api import hashes 

def toLevelKey(hash, level_num):
    return hash + ' ' + str(level_num)
def fromLevelKey(lk):
    return lk.split(' ')
def toRoomKey(hash, level_num,room_num):
    return hash + ' ' + str(level_num) + ' ' + str(room_num)
def fromRoomKey(rk):
    return rk.split(' ')

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

        lkey = toLevelKey(hash, level_num)
        if lkey not in level_results:
            level_results[lkey] = 0
        level_results[lkey] += count
    
        rkey = toRoomKey(hash, level_num, room_num)
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
        hash, level_num = fromLevelKey(lr)
        level_return.append({
            'scene_hash' : hash,
            'level_num'  : level_num,
            'value'      : value
        
        })

    room_return = []
    for rr, value in room_results.items():
        hash, level_num, room_num = fromRoomKey(rr)
        room_return.append({
            'scene_hash' : hash,
            'level_num'  : level_num,
            'room_num'   : room_num,
            'value'      : value
        })
    
    return scene_return, level_return, room_return


