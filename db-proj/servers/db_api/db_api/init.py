from flask import Flask
from flask_restful import Api
from flaskext.mysql import MySQL

import operator
from db_api import app

mysql = MySQL()
app.config['MYSQL_DATABASE_USER'] = 'root'
app.config['MYSQL_DATABASE_PASSWORD'] = 'password'
app.config['MYSQL_DATABASE_DB'] = 'scene_search'
app.config['MYSQL_DATABASE_HOST'] = 'localhost'
mysql.init_app(app)
conn = mysql.connect()
cursor = conn.cursor()

##
## Get the dictionary of hashes from the DB
##
cmd = """
SELECT
    id,
    hash
FROM
    scenes
"""

cursor.execute(cmd)
data = cursor.fetchall()
hashes = {}
for row in data:
    hashes[row[0]] = row[1]
