from flask import Flask, jsonify, send_file

from flask_restful import Resource, Api
from flask_restful.utils import cors

from webargs import fields, validate
from webargs.flaskparser import use_args, use_kwargs, parser, abort

from PIL import Image
import os
import csv

app = Flask(__name__)
api = Api(app)

def to_gray(rgb):
    r, g, b = rgb[0], rgb[1], rgb[2]
    gray = int(0.2989 * r + 0.5870 * g + 0.1140 * b)
    return gray, gray, gray, 255

def decode(color):
    return color[2] + (255*color[1]) + (color[0] * 65025)

class HighlightInstances(Resource):
    highlight_instances_args = {
        'hash': fields.Str(required=True),
        'objects': fields.List(fields.Str(), required=True)
    }
    @cors.crossdomain(origin='*')
    @use_kwargs(highlight_instances_args)
    def get(self, hash, objects):
        data_dir = './data/'+hash+'/'
        codes_filename = data_dir + 'codes.csv'
        encoded_filename = data_dir + 'encoded.png'
        color_filename = data_dir + 'color.png'
        tmp_filename = data_dir + 'tmp.png'
        
        # Identify instances to be highlighted
        labels_to_highlight = objects[0].split(',')
        indexes_to_highlight = []
        with open(codes_filename) as codes_file:
            reader = csv.reader(codes_file, delimiter=',')
            for row in reader:
                if row[1] in labels_to_highlight:
                    indexes_to_highlight.append(int(row[0]))
       
        # Manipulate the image
        encoded_im = Image.open(encoded_filename)
        encoded = encoded_im.load()

        im = Image.open(color_filename).convert('RGBA')
        pixels = im.load()

        width, height = encoded_im.size

        for i in range(0,width):
            for j in range(0,height):
                encoded_value = encoded[i,j]

                # Grey out all pixels not in objects selected
                if decode(encoded_value) not in indexes_to_highlight and encoded_value[3] != 0:
                    pixels[i,j] = to_gray(pixels[i,j])

        # Save, return, and delete temporary file
        im.save(tmp_filename)
        pkg = send_file("../" + tmp_filename, mimetype='image/png')
        os.remove(tmp_filename)
        return pkg

api.add_resource(HighlightInstances, '/getimage')

