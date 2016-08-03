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

print X
print y

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
## Learning
##

#clf = svm.SVC(gamma=0.001, C=100.)
#clf.fit(X[:-1], y[:-1]) # select all but the last
#print clf.predict(X[-1:])
#print y[-1]


##
## Model Persistence
##

from sklearn.externals import joblib # alternative to pickle
joblib.dump(clf, 'svc.pkl')

# And to load...
# clf = joblib.load('svc.pkl')
