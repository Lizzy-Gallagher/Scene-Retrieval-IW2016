def identify(tokens):
    is_found = False

    # Membership
    if is_scene_membership(tokens):
        is_found = True
        params = {
            'object' : tokens[2].as_class()
        }
        return 'sceneMembership', params, is_found

    elif is_level_membership(tokens):
        is_found = True
        params = {
            'object' : tokens[2].as_class()
        }
        return 'levelMembership', params, is_found

    elif is_room_membership(tokens):
        is_found = True
        params = {
            'roomtype': tokens[0].as_room_type(),
            'object' : tokens[2].as_class()
        }
        return 'roomMembership', params, is_found

    elif is_outside_membership(tokens):
        is_found = True
        object = tokens[0].as_class() if len(tokens[0]) == 2 else tokens[2].as_class()
        
        params = {
            'object' : object
        }
        return 'outsideMembership', params, is_found

    return '', {}, is_found

# "Scenes with chairs"
def is_scene_membership(tokens):
    if len(tokens) != 3:
        return False
    return tokens[0].is_scene_word() and tokens[1].is_contains_word() and tokens[2].is_noun()

# "Levels with chairs"
def is_level_membership(tokens):
    if len(tokens) != 3:
        return False
    return tokens[0].is_level_word() and tokens[1].is_contains_word and tokens[2].is_noun()

# "Rooms/bedrooms with chairs"
def is_room_membership(tokens):
    if len(tokens) != 3:
        return False
    return tokens[0].is_room_type_word() and tokens[1].is_contains_word() and tokens[2].is_noun()

# "Chairs outside" / "Yard with chairs"
def is_outside_membership(tokens):
    if len(tokens) == 2:
        tokens[0].is_noun() and tokens[1].is_outside_word()
    elif len(tokens) == 3:
        tokens[0].is_outside_word() and tokens[1].is_contains_word() \
            and tokens[2].is_noun()

