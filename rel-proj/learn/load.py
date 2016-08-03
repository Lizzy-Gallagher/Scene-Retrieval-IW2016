import numpy as np

arff_buffer = 6

class Dataset(object):
    def __init__(self, X, y):
        self.X = X
        self.y = y

def csv_file(filename):
    with open(filename, "r") as f:
        # count the columns
        num_cols = len(f.readline().split(","))
        
        # reset file
        f.seek(0)
        
        # read dataset
        dataset = np.genfromtxt(
            f,
            dtype=np.int,
            delimiter=",",
            usecols=range(1, num_cols))

        X = dataset[:,:-1]
        y = dataset[:,-1:].ravel()

        return Dataset(X,y)

def arff_file(filename):
    with open(filename, "r") as f:
        # count the columns
        while True:
            ln = f.readline()
            if "@" not in ln and len(ln) > 1:
                break

        num_cols = len(f.readline().split(","))
        
        # reset file
        f.seek(0)
        
        # read dataset
        dataset = np.genfromtxt(
            f,
            dtype=None,
            delimiter=",",
            skip_header = num_cols + arff_buffer,
        )
        
        print dataset.shape

        X = dataset[:,:-1]
        y = dataset[:,-1:].ravel()

        print X.shape
        print y.shape
    
        return Dataset(X,y)

