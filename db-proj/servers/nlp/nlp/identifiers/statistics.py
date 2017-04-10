def identify(tokens):
    if is_many_objects(tokens):
        if tokens[0].is_scene_word():
            return 'sceneManyObjects', {}, True
        elif tokens[0].is_level_word():
            return 'levelManyObjects', {}, True
        elif tokens[0].is_room_type_word():
            params = {
                'roomtype': tokens[0].as_room_type()
            }
            return 'roomManyObjects', params, True

    elif is_few_objects(tokens):
        if tokens[0].is_scene_word():
            return 'sceneFewObjects', {}, True
        elif tokens[0].is_level_word():
            return 'levelFewObjects', {}, True
        elif tokens[0].is_room_type_word():
            params = {
                'roomtype': tokens[0].as_room_type()
            }
            return 'roomFewObjects', params, True

    elif is_greater_area(tokens):
        if tokens[1].is_scene_word():
            return 'sceneGreaterArea', {}, True
        elif tokens[1].is_level_word():
            return 'levelGreaterArea', {}, True
        elif tokens[1].is_room_type_word():
            params = {
                'roomtype': tokens[1].as_room_type()
            }
            print params
            return 'roomGreaterArea', params, True

    elif is_lesser_area(tokens):
        if tokens[1].is_scene_word():
            return 'sceneLesserArea', {}, True
        elif tokens[1].is_level_word():
            return 'levelLesserArea', {}, True
        elif tokens[1].is_room_type_word():
            params = {
                'roomtype': tokens[1].as_room_type()
            }
            return 'roomLesserArea', params, True

    elif is_dense(tokens):
        if tokens[1].is_scene_word():
            return 'sceneDense', {}, True
        elif tokens[1].is_level_word():
            return 'levelDense', {}, True
        elif tokens[1].is_room_type_word():
            params = {
                'roomtype': tokens[1].as_room_type()
            }
            return 'roomDense', params, True

    elif is_sparse(tokens):
        if tokens[1].is_scene_word():
            return 'sceneSparse', {}, True
        elif tokens[1].is_level_word():
            return 'levelSparse', {}, True
        elif tokens[1].is_room_type_word():
            params = {
                'roomtype': tokens[1].as_room_type()
            }
            return 'roomSparse', params, True

    elif is_too_many(tokens):
        params = { 
            'object' : tokens[2].as_class()
        }
        return 'tooMany', params, True

    return '', {}, False

def is_many_objects(tokens):
    if len(tokens) == 4:
        return tokens[0].is_noun() and tokens[1].word == "with" \
                and tokens[2].is_many() and tokens[3].word == "object"
    return False

def is_few_objects(tokens):
    if len(tokens) == 4:
        return tokens[0].is_noun() and tokens[1].word == "with" \
                and tokens[2].is_few() and tokens[3].word == "object"
    return False

def is_greater_area(tokens):
    if len(tokens) == 2:
        return tokens[0].is_greater() and tokens[1].is_noun()
    return False

def is_lesser_area(tokens):
    if len(tokens) == 2:
        return tokens[0].is_lesser() and tokens[1].is_noun()
    return False

def is_dense(tokens):
    if len(tokens) == 2:
        return tokens[0].is_dense() and tokens[1].is_noun()
    return False

def is_sparse(tokens):
    if len(tokens) == 2:
        return tokens[0].is_sparse() and tokens[1].is_noun()
    return False

# "Too many chairs"
def is_too_many(tokens):
    if len(tokens) != 3:
        return False
    return tokens[0].word == "too" and tokens[1].word == "many" and tokens[2].is_noun()

