import operator

from nltk import word_tokenize, pos_tag
from nltk.stem import WordNetLemmatizer
from gensim.models import Word2Vec

wnl = WordNetLemmatizer()

# Load Word2Vec
# w2v = Word2Vec.load('./data/gutenberg_model.txt')
# w2v = Word2Vec.load('./data/brown_model.txt')

roomtypes = []
classes = []
w2v_classes = [] # those which are available in w2v

def setup():
    # Load roomtypes
    f = open('./data/list-of-rtypes.txt')
    for line in f:
        roomtypes.append(line.rstrip('\n').lower())

    # Load object classes
    f = open('./data/categories.txt')
    for line in f:
        classes.append(line.rstrip('\n').lower())

    # Identify object classes that are in the w2v model
    #for object in classes:
    #    try: 
    #        w2v[object]
    #        w2v_classes.append(object)
    #    except KeyError:
    #        continue
    #print w2v_classes

class Token(object):
    def __init__(self, raw_word, pos):
        self.raw_word = raw_word

        self.word = wnl.lemmatize(raw_word.lower(), 'n')
        self.pos = pos

        # Do Word2Vec to find nearest object class
        #if self.is_noun() and not self.is_room_type_word() and \
        #    not self.is_scene_word() and not self.is_class_word():
        #    values = []
        #    for obj in w2v_classes:
        #        values.append(w2v.similarity(obj, self.word))
        #    i = values.index(max(values))
        #    print w2v_classes[i]

    def as_class(self):
        return self.word  # eventually will return the word2vec stuff

    def as_room_type(self):
        return self.word # eventually might do word2vec?

    def as_relationship(self):
        return self.word # eventually might word to combine multi-word relationships

    # Identify POS
    def is_noun(self):
        if self.pos == "NN" or self.pos == "NNS":
            return True
        return False

    def is_preposition(self):
        if self.pos == "IN":
            return True
        return False

    # Identify "Type" of word
    def is_class_word(self):
        return self.word in classes

    def is_room_type_word(self):
        return self.word in roomtypes

    def is_scene_word(self):
        return self.word in ['scene', 'house', 'building']

    def is_level_word(self):
        return self.word in ['level', 'floor']

    def is_outside_word(self):
        return self.word in ['outside', 'outdoors', 'outdoor', 'yard']

    def is_contains_word(self):
        return self.word in ['with', 'contains', 'has']

    def is_greater(self):
        return self.word in ['big', 'large', 'huge']
    
    def is_lesser(self):
        return self.word in ['tiny', 'small', 'little']

    def is_many(self):
        return self.word in ['many']

    def is_few(self):
        return self.word in ['few']

    def is_dense(self):
        return self.word in ['dense', 'crowded']

    def is_sparse(self):
        return self.word in ['sparse']

## Actions
def getTokens(query):
    tokens = []
    for word, pos in pos_tag(word_tokenize(query)):
        tokens.append(Token(word, pos))
    return tokens

