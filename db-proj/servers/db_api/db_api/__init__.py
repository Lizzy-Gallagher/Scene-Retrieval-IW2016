from flask import Flask
from flask_restful import Api
from flaskext.mysql import MySQL

import operator

app = Flask(__name__)
api = Api(app)

##
## MySQL Database Initialization
##
from init import cursor, hashes

##
## Routes
## 
import routes

if __name__ == '__main__':
    app.run(debug=True)
