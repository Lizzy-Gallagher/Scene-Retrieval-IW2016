from sklearn import svm
import load

##
## Command-Line Argument Parsing
##

import argparse

parser = argparse.ArgumentParser()
parser.add_argument("input_filename", help="the input filename")
args = parser.parse_args()

input_filename = args.input_filename

if ".arff" not in input_filename and ".csv" not in input_filename:
    raise ValueError("Only .arff and .csv files are supported")


##
## Load Dataset
##

class Dataset(object):
    pass

dataset = Dataset()

if ".arff" in input_filename:
    dataset.data = load.arff_file(input_filename)
elif ".csv" in input_filename:
    dataset.data = load.csv_file(input_filename)

X = dataset.data.X
y = dataset.data.y

##
## Cross Validation
##

from sklearn import cross_validation
from sklearn.cross_validation import StratifiedKFold

clf = svm.SVC(kernel='linear', C=1)
scores = cross_validation.cross_val_score(clf, X, y,
                                          cv=StratifiedKFold(y,n_folds = 2))
print("Accuracy: %0.2f (+/- %0.2f)" % (scores.mean(), scores.std() * 2))


##
## Parameter Fitting
##

#  from sklearn import svm, grid_search
#  
#  print
#  print "Fitting ..."
#  print
#  
#  param_grid = [
    #  {'C': [1, 10, 100, 1000], 'kernel': ['linear']},
    #  {'C': [1, 10, 100, 1000], 'gamma': [0.001, 0.0001], 'kernel': ['rbf']},
#  ]
#  svr = svm.SVC()
#  clf = grid_search.GridSearchCV(svr, param_grid)
#  clf.fit(X, y)
#  scores = cross_validation.cross_val_score(clf, X, y,
                                          #  cv=StratifiedKFold(y,n_folds = 2))
#  print("Accuracy: %0.2f (+/- %0.2f)" % (scores.mean(), scores.std() * 2))

##
## Model Persistence
##

#from sklearn.externals import joblib # alternative to pickle
#joblib.dump(clf, 'svc.pkl')

# And to load...
# clf = joblib.load('svc.pkl')
