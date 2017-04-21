from flask import Flask, jsonify, send_file

from flask_restful import Resource, Api
from flask_restful.utils import cors

from webargs import fields, validate
from webargs.flaskparser import use_args, use_kwargs, parser, abort

from PIL import Image
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
class HighlightInstances(Resource):
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

        csv_filename = segmentation_base + hash + '.objectId.csv'
        encoded_img_filename = segmentation_base + hash + '.objectId.encoded.png'
        color_img_filename = server_base + 'scenes_rendered/' + hash + '/' + hash + '.png'
        tmp_filename = hash + '.png'

        # Identify instances to be highlighted
        labels_to_highlight = objects[0].split(',')
        indexes_to_highlight = []
        
        start_csv = time.time()
        coded_csv = requests.get(csv_filename).text.split('\n')
        reader = csv.reader(coded_csv, delimiter=',')
        for row in reader:
            if row[1] in labels_to_highlight:
                indexes_to_highlight.append(int(row[0]))
        print "Elapsed csv " + hash + " : " + str(time.time() - start_csv)

        # Manipulate the image
        start_getencodedim = time.time()
        encoded_im = Image.open(BytesIO(requests.get(encoded_img_filename).content))
        encoded = encoded_im.load()
        print "Elapsed get encoded_im " + hash + " : " + str(time.time() - start_getencodedim)

        start_getcolorim = time.time()
        color_im = Image.open(BytesIO(requests.get(color_img_filename).content)).convert('RGBA')
        pixels = color_im.load()
        print "Elapsed get color_im " + hash + " : " + str(time.time() - start_getcolorim)

        width, height = encoded_im.size
        for i in range(0,width):
            for j in range(0,height):
                encoded_value = encoded[i,j]
                # Grey out all pixels not in objects selected
                if decode(encoded_value) not in indexes_to_highlight and encoded_value[3] != 0:
                    pixels[i,j] = to_gray(pixels[i,j])

        # Save, return, and delete temporary file
        color_im.save(tmp_filename)
        pkg = send_file("../" + tmp_filename, mimetype='image/png')
        os.remove(tmp_filename)

        end = time.time()
        print "Elapsed time " + hash + " : " + str(end - start)
        return pkg

api.add_resource(HighlightInstances, '/getimage')

