def identify(tokens):
    is_found = False

    if is_single_relationship(tokens):
        is_found = True
        params = { 
            'primary' : tokens[0].as_class(), 
            'relationship' : tokens[1].as_relationship(),
            'secondary': tokens[2].as_class() 
        }
        return 'singleRelationship', params, is_found

    return '', {}, is_found

# e.g. "Person above sofa"
def is_single_relationship(tokens):
    if len(tokens) != 3:
        return False
    return tokens[0].is_noun() and tokens[1].is_preposition() and tokens[2].is_noun()

