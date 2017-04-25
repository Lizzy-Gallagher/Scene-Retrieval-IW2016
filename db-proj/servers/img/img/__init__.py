from flask import Flask, jsonify, send_file

from flask_restful import Resource, Api
from flask_restful.utils import cors

from webargs import fields, validate
from webargs.flaskparser import use_args, use_kwargs, parser, abort

import numpy as np
import scipy.misc

import os
import csv
import requests
from io import BytesIO

app = Flask(__name__)
api = Api(app)

def to_gray(rgb):
    r, g, b = rgb[0], rgb[1], rgb[2]
    gray = int(0.2989 * r + 0.5870 * g + 0.1140 * b)
    return gray, gray, gray, 255

def decode(color):
    return color[2] + (255*color[1]) + (color[0] * 65025)

import time
class HighlightInstancesScene(Resource):
    highlight_instances_args = {
        'hash': fields.Str(required=True),
        'objects': fields.List(fields.Str(), required=True)
    }
    @cors.crossdomain(origin='*')
    @use_kwargs(highlight_instances_args)
    def get(self, hash, objects):
        start = time.time()

        server_base = 'http://dovahkiin.stanford.edu/fuzzybox/suncg/planner5d/'
        segmentation_base = server_base + 'object_mask/house/' + hash + '/'

        csv_filename = 'data/objectId_csv/' + hash + '.objectId.csv'
        encoded_img_filename = segmentation_base + hash + '.objectId.encoded.png'
        color_img_filename = server_base + 'scenes_rendered/' + hash + '/' + hash + '.png'
        tmp_filename = hash + '.png'

        # Identify instances to be highlighted
        labels_to_highlight = objects[0].split(',')
        indexes_to_highlight = {}
        
        with open(csv_filename, 'r') as csv_file:
            reader = csv.reader(csv_file, delimiter=',')
            for row in reader:
                if row[1] in labels_to_highlight:
                    indexes_to_highlight[int(row[0])] = ""

        # Manipulate the image
        start_getencodedim = time.time()

        # Manipulate the image
        start_getencodedim = time.time()
        encoded_im = scipy.misc.imread(BytesIO(requests.get(encoded_img_filename).content))
        print "Elapsed get encoded_im " + hash + " : " + str(time.time() - start_getencodedim)

        start_getcolorim = time.time()
        color_im = scipy.misc.imread(BytesIO(requests.get(color_img_filename).content))
        print "Elapsed get color_im " + hash + " : " + str(time.time() - start_getcolorim)
        
        start_computation = time.time()
        R, G, B, A = encoded_im[:,:,0], encoded_im[:,:,1], encoded_im[:,:,2], encoded_im[:,:,3]
        r, g, b, a = color_im[:,:,0], color_im[:,:,1], color_im[:,:,2], color_im[:,:,3]

        gray = 0.2989 * r + 0.5870 * g + 0.1140 * b
        decoded = B + 255 * G + R * 65025 
        
        decoded = np.in1d(decoded, indexes_to_highlight.keys())
        decoded = np.reshape(decoded, R.shape)

        truth = np.where(decoded, True, np.where(A == 0, True, False))
        
        r = np.where(decoded, r, np.where(A == 0, 255, gray))
        g = np.where(decoded, g, np.where(A == 0, 255, gray))
        b = np.where(decoded, b, np.where(A == 0, 255, gray))

        color_im = np.dstack((r, g, b))

        # Save, return, and delete temporary file
        scipy.misc.imsave(tmp_filename, color_im)
        pkg = send_file("../" + tmp_filename, mimetype='image/png')
        os.remove(tmp_filename)

        end = time.time()
        print "Elapsed time " + hash + " : " + str(end - start)
        return pkg

class HighlightInstancesLevel(Resource):
    highlight_instances_args = {
        'hash': fields.Str(required=True),
        'level_num' : fields.Str(required=True),
        'objects': fields.List(fields.Str(), required=True)
    }
    @cors.crossdomain(origin='*')
    @use_kwargs(highlight_instances_args)
    def get(self, hash, level_num, objects):
        start = time.time()

        server_base = 'http://dovahkiin.stanford.edu/fuzzybox/suncg/planner5d/'
        segmentation_base = server_base + 'object_mask/room/' + hash + '/' + \
            hash + '_' + level_num

        csv_filename = 'data/objectId_csv/' + hash + '.objectId.csv'
        encoded_img_filename = segmentation_base + '.objectId.encoded.png'
        color_img_filename = server_base + 'rooms_rendered/' + hash + '/' + \
                hash + '_' + level_num + '.png'
        tmp_filename = hash + level_num + '.png'

        print csv_filename
        print encoded_img_filename
        print color_img_filename

        # Identify instances to be highlighted
        labels_to_highlight = objects[0].split(',')
        indexes_to_highlight = {}
        
        with open(csv_filename, 'r') as csv_file:
            reader = csv.reader(csv_file, delimiter=',')
            for row in reader:
                if row[1] in labels_to_highlight:
                    indexes_to_highlight[int(row[0])] = ""

        # Manipulate the image
        start_getencodedim = time.time()
        encoded_im = scipy.misc.imread(BytesIO(requests.get(encoded_img_filename).content))
        print "Elapsed get encoded_im " + hash + " : " + str(time.time() - start_getencodedim)

        start_getcolorim = time.time()
        color_im = scipy.misc.imread(BytesIO(requests.get(color_img_filename).content))
        print "Elapsed get color_im " + hash + " : " + str(time.time() - start_getcolorim)
        
        R, G, B, A = encoded_im[:,:,0], encoded_im[:,:,1], encoded_im[:,:,2], encoded_im[:,:,3]
        r, g, b, a = color_im[:,:,0], color_im[:,:,1], color_im[:,:,2], color_im[:,:,3]

        gray = 0.2989 * r + 0.5870 * g + 0.1140 * b
        decoded = B + 255 * G + R * 65025 
        
        decoded = np.in1d(decoded, indexes_to_highlight.keys())
        decoded = np.reshape(decoded, R.shape)

        truth = np.where(decoded, True, np.where(A == 0, True, False))
        
        r = np.where(decoded, r, np.where(A == 0, 255, gray))
        g = np.where(decoded, g, np.where(A == 0, 255, gray))
        b = np.where(decoded, b, np.where(A == 0, 255, gray))

        color_im = np.dstack((r, g, b))

        # Save, return, and delete temporary file
        scipy.misc.imsave(tmp_filename, color_im)
        pkg = send_file("../" + tmp_filename, mimetype='image/png')
        os.remove(tmp_filename)

        end = time.time()
        print "Elapsed time " + hash + " : " + str(end - start)
        return pkg



class HighlightInstancesRoom(Resource):
    highlight_instances_args = {
        'hash': fields.Str(required=True),
        'level_num' : fields.Str(required=True),
        'room_num' : fields.Str(required=True),
        'objects': fields.List(fields.Str(), required=True)
    }
    @cors.crossdomain(origin='*')
    @use_kwargs(highlight_instances_args)
    def get(self, hash, level_num, room_num, objects):
        start = time.time()

        server_base = 'http://dovahkiin.stanford.edu/fuzzybox/suncg/planner5d/'
        segmentation_base = server_base + 'object_mask/room/' + hash + '/' + \
            hash + '_' + level_num + '_' + room_num

        csv_filename = 'data/objectId_csv/' + hash + '.objectId.csv'
        encoded_img_filename = segmentation_base + '.objectId.encoded.png'
        color_img_filename = server_base + 'rooms_rendered/' + hash + '/' + \
                hash + '_' + level_num + '_' + room_num + '.png'
        tmp_filename = hash + level_num + room_num+ '.png'

        print csv_filename
        print encoded_img_filename
        print color_img_filename

        # Identify instances to be highlighted
        labels_to_highlight = objects[0].split(',')
        indexes_to_highlight = {}
        
        with open(csv_filename, 'r') as csv_file:
            reader = csv.reader(csv_file, delimiter=',')
            for row in reader:
                if row[1] in labels_to_highlight:
                    indexes_to_highlight[int(row[0])] = ""

        # Manipulate the image
        start_getencodedim = time.time()
        encoded_im = scipy.misc.imread(BytesIO(requests.get(encoded_img_filename).content))
        print "Elapsed get encoded_im " + hash + " : " + str(time.time() - start_getencodedim)

        start_getcolorim = time.time()
        color_im = scipy.misc.imread(BytesIO(requests.get(color_img_filename).content))
        print "Elapsed get color_im " + hash + " : " + str(time.time() - start_getcolorim)
        
        R, G, B, A = encoded_im[:,:,0], encoded_im[:,:,1], encoded_im[:,:,2], encoded_im[:,:,3]
        r, g, b, a = color_im[:,:,0], color_im[:,:,1], color_im[:,:,2], color_im[:,:,3]

        gray = 0.2989 * r + 0.5870 * g + 0.1140 * b
        decoded = B + 255 * G + R * 65025 
        
        decoded = np.in1d(decoded, indexes_to_highlight.keys())
        decoded = np.reshape(decoded, R.shape)

        truth = np.where(decoded, True, np.where(A == 0, True, False))
        
        r = np.where(decoded, 255, np.where(A == 0, 255, gray))
        g = np.where(decoded, 48, np.where(A == 0, 255, gray))
        b = np.where(decoded, 48, np.where(A == 0, 255, gray))

        color_im = np.dstack((r, g, b))

        # Save, return, and delete temporary file
        scipy.misc.imsave(tmp_filename, color_im)
        pkg = send_file("../" + tmp_filename, mimetype='image/png')
        os.remove(tmp_filename)

        end = time.time()
        print "Elapsed time " + hash + " : " + str(end - start)
        return pkg

class Ping(Resource):
    @cors.crossdomain(origin='*')
    def get(self):
        return jsonify({'status':'ok!'}), 200

api.add_resource(Ping, '/ping')
api.add_resource(HighlightInstancesScene, '/highlightscene')
api.add_resource(HighlightInstancesLevel, '/highlightlevel')
api.add_resource(HighlightInstancesRoom, '/highlightroom')

