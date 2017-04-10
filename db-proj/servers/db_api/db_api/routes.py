from db_api import api
from resources import *

##
## Api Resource Routing
##

api.add_resource(DatabaseStats,'/numScenes')
api.add_resource(SingleRelationship, '/singleRelationship')

api.add_resource(SceneMembership, '/sceneMembership')
api.add_resource(LevelMembership, '/levelMembership') 
api.add_resource(RoomMembership, '/roomMembership')
api.add_resource(OutdoorMembership, '/outdoorMembership')

api.add_resource(TooMany, '/tooMany')

api.add_resource(SceneGreaterArea, '/sceneGreaterArea')
api.add_resource(SceneLesserArea, '/sceneLesserArea')
api.add_resource(LevelGreaterArea, '/levelGreaterArea')
api.add_resource(LevelLesserArea, '/levelLesserArea')
api.add_resource(RoomGreaterArea, '/roomGreaterArea')
api.add_resource(RoomLesserArea, '/roomLesserArea')

api.add_resource(SceneManyObjects, '/sceneManyObjects')
api.add_resource(SceneFewObjects, '/sceneFewObjects')
api.add_resource(SceneEmpty, '/sceneEmpty')
api.add_resource(LevelManyObjects, '/levelManyObjects')
api.add_resource(LevelFewObjects, '/levelFewObjects')
api.add_resource(LevelEmpty, '/levelEmpty')
api.add_resource(RoomManyObjects, '/roomManyObjects')
api.add_resource(RoomFewObjects, '/roomFewObjects')
api.add_resource(RoomEmpty, '/roomEmpty')

api.add_resource(SceneDense, '/sceneDense')
api.add_resource(SceneSparse, '/sceneSparse')
api.add_resource(LevelDense, '/levelDense')
api.add_resource(LevelSparse, '/levelSparse')
api.add_resource(RoomDense, '/roomDense')
api.add_resource(RoomSparse, '/roomSparse')
