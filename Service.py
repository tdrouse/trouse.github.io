#!/usr/bin/python
import json
from bson import json_util
import bottle
from bottle import route, run, request, abort
import datetime
from datetime import timedelta
from pymongo import MongoClient

client = MongoClient()
db = client['city']
collection = db['inspections']

strings = [
    {
        'first': 'first',
        'second': 'second'
    }
]

# # set up URI paths for REST service
@route('/hello', method='GET')
def get_string():
  dateString="world"
  string="{hello:\""+dateString+"\"}"
  return json.loads(json.dumps(string, indent=4, default=json_util.default))


@route('/strings', method='POST')
def create_strings():
  if not request.json or not 'string1' in request.json:
      abort(400)
  string = {
      'first': request.json['string1'],
      'second': request.json['string2']
  }
  strings.append(string)
  return json.loads(json.dumps(string, indent=4, default=json_util.default))

## Restful service
@route('/read', method='GET')
def get_inspection(business_name):
  try:
    result=collection.find(business_name)
  except ValidationError as ve:
    print('Error with find')
    abort(404)
    return json.loads(json.dumps(result, indent=4, default=json_util.default))


@route('/create', method='POST')
def create_inspection(document): 
  try:
    result=collection.save(document)
  except ValidationError as ve:
    print('Error with insert')
    abort(404)
  return json.loads(json.dumps(result, indent=4, default=json_util.default))


@route('/update', method='PUT')
def update_inspection(id, update):
  try:
    result=collection.update(id, update)
  except ValidationError as ve:
    print('Error with update')
    abort(404)
  return json.loads(json.dumps(result, indent=4, default=json_util.default))


@route('/delete', method='DELETE')
def delete_inspection(id):
  try:
    result=collection.delete_one(id)
  except ValidationError as ve:
    print('Error with delete')
    abort(404)
    return jsonify( { 'result': True } )

  
if __name__ == '__main__':
  #app.run(debug=True)
  run(host='localhost', port=8080)