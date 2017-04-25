class State(object):
    def get_name(self):
        return self.__class__.__name__
    def get_suggestions(self):
        raise NotImplementedError(self.get_name() + ' has no suggestions')

def mod_phrase(phrase, do_include):
    if do_include:
        phrase = '+' + phrase
    else:
        phrase = '-' + phrase
    return phrase

def createAdjPhrase(adjective, location, do_include=True):
    phrase = "Adjective('%s', '%s')" % (adjective, location)
    return mod_phrase(phrase, do_include)

def createRelPhrase(primary_object, relationship, secondary_object, do_include=True):
    phrase = "Relationship('%s', '%s', '%s')" % (primary_object, relationship, \
                                               secondary_object)
    return mod_phrase(phrase, do_include)


def createContainPhrase(location, object, do_include=True):
    phrase = "Contains('%s', '%s')" % (location, object)
    return mod_phrase(phrase, do_include)

def createStatsPhrase(location, adjective, object, do_include=True):
    phrase = "Stats('%s', '%s', '%s')" % (location, adjective, object)
    return mod_phrase(phrase, do_include)

class AndState(State):
    def __init__(self, previous_state, dfa):
        self.previous_state = previous_state
        self.dfa = dfa

    def next(self, token):
        if token in self.dfa.articles:
            return self
        elif type(self.previous_state) == Relationship:
            if token in self.dfa.objects:
                return self.previous_state.next(token)
        elif type(self.previous_state) == StartLocation:
            return StartState(self.dfa).next(token)
        elif type(self.previous_state) == LocationContaining:
            if token in self.dfa.objects:
                return self.previous_state.next(token)
        
        elif type(self.previous_state) == StartAdjective:
            if token in self.dfa.adjectives:
                self.previous_state.adjectives.extend([token])
                new_adjectives = self.previous_state.adjectives
                return StartAdjective(new_adjectives, self.dfa)

        elif type(self.previous_state) == HoldAdjective:
            if token in self.dfa.containing_words:
                return LocationContaining(self.previous_state.location, self.dfa)
            elif token in self.dfa.anti_containing_words:
                return LocationContaining(self.previous_state.location, self.dfa, False)
            else:
                return StartState(self.dfa).next(token)
        return UnexpectedState(self.dfa)

    def get_suggestions(self):
       # TODO (should be dependent on scene)
        suggestions = [{
            'title': 'location',
            'suggestions': self.dfa.locations
        },
        {
            'title': 'objects',
            'suggestions': self.dfa.objects
        },
        {
            'title': 'adjectives',
            'suggestions': self.dfa.adjectives
        }]
        return suggestions

class ButState(State):
    def __init__(self, previous_state, dfa):
        self.previous_state = previous_state
        self.dfa = dfa

    def next(self, token):
        if token in self.dfa.articles:
            return self
        elif type(self.previous_state) == LocationContaining:
            if token in self.dfa.objects:
                return self.previous_state.next(token, False)

        return UnexpectedState(self.dfa)

class StartState(State):
    def __init__(self, dfa):
        self.dfa = dfa

    def next(self, token):
        if token in self.dfa.articles:
            return self
        elif token in self.dfa.locations:
            return StartLocation(token, self.dfa)
        elif token in self.dfa.objects:
            return StartObject(token, self.dfa)
        elif token in self.dfa.adjectives:
            return StartAdjective([token], self.dfa)
        return UnexpectedState(self.dfa)

    def get_suggestions(self):
        suggestions = [{
            'title': 'location',
            'suggestions': self.dfa.locations
        },
        {
            'title': 'objects',
            'suggestions': self.dfa.objects
        },
        {
            'title': 'adjectives',
            'suggestions': self.dfa.adjectives
        }]
        return suggestions


class StartAdjective(State):
    def __init__(self, adjectives, dfa):
        self.adjectives = adjectives
        self.dfa = dfa

    def next(self, token):
        if token in self.dfa.locations:
            for adj in self.adjectives:
                phrase = createAdjPhrase(adj, token)
                self.dfa.append_call(phrase)
            return StartLocation(token, self.dfa)
        elif token == 'and':
            return AndState(self, self.dfa)
        return UnexpectedState(self.dfa)

    def get_suggestions(self):
        suggestions = [{
            'title': 'locations',
            'suggestions': self.dfa.locations
        },
        {
            'title': 'conjunctions',
            'suggestions': ['and']
        }]
        return suggestions


class StartObject(State):
    def __init__(self, primary_object, dfa):
        self.primary_object = primary_object
        self.dfa = dfa

    def next(self, token):
        if token in self.dfa.relationships:
            return Relationship(self.primary_object, token, self.dfa)
        return UnexpectedState(self.dfa)

    def get_suggestions(self):
        suggestions = [{
            'title': 'relationships',
            'suggestions': self.dfa.relationships
        }]
        return suggestions

class Relationship(State):
    def __init__(self, primary_object, relationship, dfa):
        self.primary_object = primary_object
        self.relationship = relationship
        self.dfa = dfa

    def next(self, token):
        if token in self.dfa.articles:
            return self
        elif token in self.dfa.objects:
            self.dfa.append_call(createRelPhrase(self.primary_object,
                                                 self.relationship,
                                                 token))
            return self
        elif token == 'and':
            return AndState(self, self.dfa)
        return UnexpectedState(self.dfa)
    
    def get_suggestions(self):
        suggestions = [{
            'title': 'objects',
            'suggestions': self.dfa.objects
        },
        {
            'title': 'conjunctions',
            'suggestions': ['and']
        }]
        return suggestions


class HoldAdjective(State):
    def __init__(self, location, dfa):
        self.location = location
        self.dfa = dfa

    def next(self, token):
        if token == 'and':
            return AndState(self, self.dfa)
        elif token in self.dfa.containing_words:
            return LocationContaining(self.location, self.dfa)
        elif token in self.dfa.anti_containing_words:
            return LocationContaining(self.location, self.dfa, False)
        return UnexpectedState(self.dfa)
    
    def get_suggestions(self):
        suggestions = [{
            'title': 'containment',
            'suggestions': self.dfa.containing_words
        },
        {
            'title': 'not-containment',
            'suggestions': self.dfa.anti_containing_words
        },
        {
            'title': 'conjunctions',
            'suggestions': ['and']
        }]
        return suggestions


class AwaitAdjective(State):
    def __init__(self, location, dfa):
        self.location = location
        self.dfa = dfa

    def next(self, token):
        if token in self.dfa.adjectives:
            self.dfa.append_call(createAdjPhrase(token, self.location))
            return HoldAdjective(self.location, self.dfa)
        return UnexpectedState(self.dfa)
    
    def get_suggestions(self):
        suggestions = [{
            'title': 'adjectives',
            'suggestions': self.dfa.adjectives
        }]
        return suggestions


class StartLocation(State):
    def __init__(self, location, dfa):
        self.location = location
        self.dfa = dfa

    def next(self, token):
        print token
        if token in self.dfa.containing_words:
            return LocationContaining(self.location, self.dfa)
        elif token in self.dfa.anti_containing_words:
            return LocationContaining(self.location, self.dfa, False)
        elif token == 'relative_pronoun':
            return AwaitAdjective(self.location, self.dfa)
        elif token == 'and': # coming from StartAdjective
            return AndState(self, self.dfa)
        return UnexpectedState(self.dfa)
    
    def get_suggestions(self):
        suggestions = [{
            'title': 'containment',
            'suggestions': self.dfa.containing_words
        },
        {
            'title': 'not-containment',
            'suggestions': self.dfa.anti_containing_words
        },
        {
            'title': 'conjunctions',
            'suggestions': ['and']
        }]
        return suggestions


class LocationContaining(State):
    def __init__(self, location, dfa, do_include=True):
        self.location = location
        self.dfa = dfa
        self.do_include = do_include

    def next(self, token, to_include=None):
        if to_include != None:
            self.do_include = to_include
        if token in self.dfa.articles:
            return self
        elif token in self.dfa.objects:
            self.dfa.append_call(createContainPhrase(self.location, token, self.do_include))
            return self
        elif token == 'and':
            return AndState(self, self.dfa)
        elif token in self.dfa.negation:
            return ButState(self, self.dfa)

        elif token in self.dfa.count_adjectives:
            return LocationCount(self.location, token, self.dfa)
        return UnexpectedState(self.dfa)

    def get_suggestions(self):
        suggestions = [{
            'title': 'objects',
            'suggestions': self.dfa.objects
        },
        {
            'title': 'misc',
            'suggestions': ['objects']
        },
        {
            'title': 'adjectives',
            'suggestions': self.dfa.count_adjectives
        },
        {
            'title': 'conjunctions',
            'suggestions': ['and']
        }]
        return suggestions

# be able to handle too many / outlier
class LocationCount(State):
    def __init__(self, location, count_adjective, dfa):
        self.location = location
        self.count_adjective = count_adjective
        self.dfa = dfa

    def next(self, token):
        print token
        print token in self.dfa.objects
        if token in self.dfa.objects or token == 'object':
            self.dfa.append_call(createStatsPhrase(self.location,
                                                   self.count_adjective,
                                                   token)) 
            return self
        elif token == 'and':
            return LocationContaining(self.location, self.dfa)
        return UnexpectedState(self.dfa)

    def get_suggestions(self):
        suggestions = [{
            'title': 'objects',
            'suggestions': self.dfa.objects
        },
        {
            'title': 'misc',
            'suggestions': ['objects']
        },
        {
            'title': 'conjunctions',
            'suggestions': ['and']
        }]
        return suggestions

class UnexpectedState(State):
    def __init__(self, dfa):
        dfa.in_unexpected_state = True

    def next(self, token):
        return self


