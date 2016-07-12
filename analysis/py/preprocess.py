import csv

##
## I/O
##

primary = 

def read_stats():
    """ Read CSV file, create and update relationships """
    with open(input_filename) as csvfile:
        reader = csv.reader(csvfile, delimiter=',')
        for row in reader:
            print (', '.join(row)) # TODO

def write_stats():
    """ Write statistics """
    with open(output_filename, 'w+') as outputfile:
        writer = csv.writer(outputfile, delimiter=' ')
        writer.writerow(['a', 'b', 'c'])


def preprocess():
    # for every category of ref_objects, average all numeric values, this is
    # the new line
    
    # use AWK to do this


    raise NotImplementedError()

