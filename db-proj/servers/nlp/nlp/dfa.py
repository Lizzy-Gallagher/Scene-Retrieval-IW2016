# Eventually make a DFA state class


import inflection
from states import *
from ast import literal_eval

class DFA(object):
    roomtypes = []
    objects = []
    in_unexpected_state = False

    def __init__(self):
        print "Initializing DFA..."

        if len(self.roomtypes) == 0:
            # Load roomtypes
            f = open('data/list-of-rtypes.txt')
            for line in f:
                self.roomtypes.append(line.rstrip('\n').lower())

        if len(self.objects) == 0:
            # Load State classes
            f = open('data/categories.txt')
            for line in f:
                self.objects.append(line.rstrip('\n').lower())

        self.state = StartState(self)
        self.calls = []

        self.articles = ['a', 'an', 'the']
        self.locations = ['scene', 'level', 'yard', 'room']

        self.containing_words = ['with', 'contains', 'has']
        self.anti_containing_words = ['without']

        self.negation = ['negation', 'but']

        self.count_adjectives = ['many', 'few', 'no']
        self.relationships = ['above']

        self.scene_words = ['scene', 'house', 'building']
        self.level_words = ['level', 'floor']
        self.outside_words = ['outside', 'outdoors', 'outdoor', 'yard']

        self.greater = ['big', 'large', 'huge']
        self.lesser  = ['tiny', 'small', 'little']
        self.many    = ['many']
        self.few     = ['few']
        self.no      = ['no']
        self.dense   = ['dense', 'crowded']
        self.sparse  = ['sparse']

        self.adjectives = []
        self.adjectives.extend(self.greater)
        self.adjectives.extend(self.lesser)
        self.adjectives.extend(self.dense)
        self.adjectives.extend(self.sparse)

        self.locations = []
        self.locations.extend(self.scene_words)
        self.locations.extend(self.level_words)
        self.locations.extend(self.outside_words)
        self.locations.extend(self.roomtypes)

        print "\tDone!"

    def next(self, token):
        token = inflection.singularize(token)
        
        # Fixing nonsensical singularization
        if token in ['ha', 'contain', 'shoe']:
            token += 's'
        
        self.state, call = self.state.next(token)
        
        if type(self.state) is UnexpectedState:
            print "UnexpectedState!"

        if call is not None:
            self.calls.append(call)
    
    def append_call(self, call):
        self.calls.append(call)

    def get_calls(self):
        return self.calls

    def translate_calls(self):
        translated = []

        for call in self.calls:
            do_include = 1 if call[0] == '+' else -1
            print do_include

            call = call[1:]
            type = call.split('(')[0]
            call = '(' + call.split('(')[1]
            print type
            print literal_eval(call)
            literal_params = literal_eval(call)

            if type == 'Contains':
                params = {
                    'object' : literal_params[1]
                }
                location = literal_params[0]
                if location in self.scene_words:
                    translated.append(('sceneMembership', params, do_include))
                elif location in self.level_words:
                    translated.append(('levelMembership', params, do_include))
                elif location in self.outside_words:
                    translated.append(('outdoorMembership', params, do_include))
                elif location in self.roomtypes:
                    params['roomtype'] = location
                    translated.append(('roomMembership', params, do_include))

            elif type == 'Relationship':
                params = {
                    'primary': literal_params[0],
                    'relationship': literal_params[1],
                    'secondary': literal_params[2]
                }
                translated.append(('singleRelationship', params, do_include))

            elif type == 'Adjective':
                adjective = literal_params[0]
                location = literal_params[1]

                if location in self.scene_words:
                    if adjective in self.greater:
                        translated.append(('sceneGreaterArea', {}, do_include))
                    elif adjective in self.lesser:
                        translated.append(('sceneLesserArea', {}, do_include))
                    elif adjective in self.dense:
                        translated.append(('sceneDense', {}, do_include))
                    elif adjective in self.sparse:
                        translated.append(('sceneSparse', {}, do_include))
                    else:
                        raise SystemExit

                elif location in self.level_words:
                    if adjective in self.greater:
                        translated.append(('levelGreaterArea', {}, do_include))
                    elif adjective in self.lesser:
                        translated.append(('levelLesserArea', {}, do_include))
                    elif adjective in self.dense:
                        translated.append(('levelDense', {}, do_include))
                    elif adjective in self.sparse:
                        translated.append(('levelSparse', {}, do_include))
                    else:
                        raise SystemExit

                elif location in self.roomtypes:
                    params = {
                        'roomtype': location
                    }
                    if adjective in self.greater:
                        translated.append(('roomGreaterArea', params, do_include))
                    elif adjective in self.lesser:
                        translated.append(('roomLesserArea', params, do_include))
                    elif adjective in self.dense:
                        translated.append(('roomDense', params, do_include))
                    elif adjective in self.sparse:
                        translated.append(('roomSparse', params, do_include))
                    else:
                        SystemExit

            elif type == 'Stats':
                location = literal_params[0]
                adjective = literal_params[1]
                object    = literal_params[2]

                if location in self.scene_words:
                    if adjective in self.many:
                        if object == 'object':
                            translated.append(('sceneManyObjects', {}, do_include))
                    elif adjective in self.few:
                        if object == 'object':
                            translated.append(('sceneFewObjects', {}, do_include))
                    elif adjective in self.no:
                        if object == 'object':
                            translated.append(('sceneEmpty', {}, do_include))

                elif location in self.level_words:
                    if adjective in self.many:
                        if object == 'object':
                            translated.append(('levelManyObjects', {}, do_include))
                    elif adjective in self.few:
                        if object == 'object':
                            translated.append(('levelFewObjects', {}, do_include))
                    elif adjective in self.no:
                        if object == 'object':
                            translated.append(('levelEmpty', {}, do_include))

                elif location in self.roomtypes:
                    params = {
                        'roomtype': location
                    }
                    if adjective in self.many:
                        if object == 'object':
                            translated.append(('roomManyObjects', params, do_include))
                    elif adjective in self.few:
                        if object == 'object':
                            translated.append(('roomFewObjects', params, do_include))
                    elif adjective in self.no:
                        if object == 'object':
                            translated.append(('roomEmpty', params, do_include))

        print translated
        return translated

if __name__ == '__main__':
    #q = 'scene with a sofa and a bed'
    #q = 'room with few beds'
    #q = 'sofa above bed and chair'
    #q = 'scene with bed and a chair'
    q = 'small scene'
    tokens = q.split(' ')
    dfa = DFA()

    for token in tokens:
        dfa.next(token)

    for call in dfa.get_calls():
        print call

    dfa.translate_calls()
