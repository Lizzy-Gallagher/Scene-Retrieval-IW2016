from flask import Flask, jsonify

from flask_restful import Resource, Api
from flask_restful.utils import cors

from webargs import fields, validate
from webargs.flaskparser import use_args, use_kwargs, parser, abort

from tokens import setup, getTokens
import identifiers
import dfa

app = Flask(__name__)
api = Api(app)

setup()

import re
def separate_queries(query):
    segments = re.split('(and|but not|but)', query)

    queries = []

    queries.append((segments[0], 1))
    i = 1
    while i < len(segments):
        if segments[i] == 'and':
            queries.append((segments[i+1].strip(), 1))
        elif segments[i] == 'but' or segments[i] == 'but not':
            queries.append((segments[i+1].strip(), -1))
        i += 1

    return queries

def create_api_call(name, params, to_include):
    return {'apiCall' : name,
            'params'  : params,
            'toInclude' : to_include}

def process_tokens(query):
    query = query.lower()

    replacements = {
        # sentence constructions
        'that is': 'relative_pronoun',
        'that are': 'relative_pronoun',
        "that's": 'relative_pronoun',
        "that're": 'relative_pronoun',

        # negations
        'but not': 'negation',
        'does not': 'negation',
        'do not': 'negation',
        "don't": 'negation',
        "doesn't": 'negation',

        # rtypes
        'living room': 'living_room',
        'child room': 'child_room',
        'dining room': 'dining_room',
        'guest room': 'guest_room',
        'boiler room': 'boiler_room',
        'passenger elevator': 'passenger_elevator',
        'freight elevator': 'freight_elevator',

        # coarse categories
        'air conditioner': 'air_conditioner',
        'bathroom stuff': 'bathroom_stuff',
        'bench': 'bench_chair',
        'bench chair': 'bench_chair',
        'dressing table': 'dressing_table',
        'bar': 'drinkbar',
        'garage door': 'garage_door',
        'gym equipment': 'gym_equipment',
        'hanging kitchen cabinet': 'hanging_kitchen_cabinet',
        'hanging kitchen_cabinet': 'hanging_kitchen_cabinet',
        'houshold appliance': 'household_appliance',
        'indoor lamp': 'indoor_lamp',
        'kitchen appliance': 'kitchen_appliance',
        'kitchen cabinet': 'kitchen_cabinet',
        'kitchen set': 'kitchen_set',
        'outdoor cover': 'outdoor_cover',
        'outdoor lamp': 'outdoor_lamp',
        'outdoor seating': 'outdoor_seating',
        'outdoor spring': 'outdoor_spring',
        'picture frame': 'picture_frame',
        'shoes cabinet': 'shoes_cabinet',
        'shoe cabinet': 'shoes_cabinet',
        'storage bench': 'storage_bench',
        'table and chairs': 'table_and_chair',
        'table and chair': 'table_and_chair',
        'trash can': 'trash_can',
        'tv stand': 'tv_stand',
        'wardrobe': 'wardrobe_cabinet',
        'wardrobe cabinet': 'wardrobe_cabinet',

        # fine categories
        'baby bed': 'baby_bed',
        'basketball hoop': 'basketball_hoop',
        'bunker bed': 'bunker_bed',
        'ceiling fan': 'ceiling_fan',
        'chair set': 'chair_set',
        'coffee kettle': 'coffee_kettle',
        'kettle': 'coffee_kettle',
        'coffee machine': 'coffee_machine',
        'coffee table': 'coffee_table',
        'cutting board': 'cutting_board',
        'dining table': 'dining_table',
        'dining_room table': 'dining_table',
        'dining room table': 'dining_table',
        'double bed': 'double_bed',
        'fish tank': 'fish_tank',
        'floor lamp': 'floor_lamp',
        'food processor': 'food_processor',
        'food tray': 'food_tray',
        'fruit bowl': 'fruit_bowl',
        'game table': 'game_table',
        'goal post': 'goal_post',
        'hair dryer': 'hair_dryer',
        'headphones on stand': 'headphones_on_stand',
        'headphones': 'headphones_on_stand',
        'ironing board': 'ironing_board',
        'knife rack': 'knife_rack',
        'lawn mower': 'lawn_mower',
        'mortar and pestle': 'mortar_and_pestle',
        'office chair': 'office_chair',
        'pedestal fan': 'pedestal_fan',
        'place setting': 'place_setting',
        'poker chips': 'poker_chips',
        'range hood': 'range_hood',
        'range hood with cabinet': 'range_hood_with_cabinet',
        'range_hood with cabinet': 'range_hood_with_cabinet',
        'range oven': 'range_oven',
        'range oven with cabinet': 'range_oven_with_cabinet',
        'range_oven with cabinet': 'range_oven_with_cabinet',
        'rifle on wall': 'rifle_on_wall',
        'single bed': 'single_bed',
        'slot machine': 'slot_machine',
        'slot machine and chair': 'slot_machine_and_chair',
        'slot_machine and chair': 'slot_machine_and_chair',
        'soap dish': 'soap_dish',
        'soap dispenser': 'soap_dispenser',
        'stationary container': 'stationary_container',
        'surveillance camera': 'surveillance_camera',
        'table lamp': 'table_lamp',
        'toilet paper': 'toilet_paper',
        'toilet plunger': 'toilet_plunger',
        'plunger': 'toilet plunger',
        'towel hanger': 'towel_hanger',
        'towel rack': 'towel_rack',
        'trash can': 'trash_can',
        'utensil holder': 'utensil_holder',
        'vacuum cleaner': 'vacuum_cleaner',
        'vacuum': 'vacuum_cleaner',
        'wall lamp': 'wall_lamp',
        'water dispenser': 'water_dispenser',
        'weight scale': 'weight_scale',
        'wood board': 'wood_board'
    }

    for key in replacements.keys():
        if key in query:
            query = query.replace(key, replacements[key])

    return query.split(' ')

class ApiCall(Resource):
    apicall_args = {
        'query': fields.Str(required=True)
    }

    @cors.crossdomain(origin='*')
    @use_kwargs(apicall_args)
    def get(self, query):
        calls = []
        tokens = process_tokens(query)

        dfa_ = dfa.DFA()

        for token in tokens:
            print token
            dfa_.next(token)
            print dfa_.state

        for call in dfa_.get_calls():
            print call

        for call in dfa_.translate_calls():
            calls.append(create_api_call(call[0], call[1], call[2]))

        if len(calls) != 0:
            return jsonify({'apiCalls' : calls}), 200

        return jsonify({'error' : 'Please try again'}), 400

class Autocomplete(Resource):
    autocomplete_args = {
        'query': fields.Str(required=True)
    }
    @cors.crossdomain(origin='*')
    @use_kwargs(autocomplete_args)
    def get(self, query):
        tokens = process_tokens(query)
        dfa_ = dfa.DFA()

        for token in tokens:
            dfa_.next(token)

        print dfa_.state
        
        ret = {
            'state': dfa_.state.get_name(), # For testing
            'suggestions': dfa_.state.get_suggestions()
        }

        return jsonify(ret), 200

api.add_resource(ApiCall, '/api')
api.add_resource(Autocomplete, '/autocomplete')
