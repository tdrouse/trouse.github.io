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

## Restful service
## read
@route('/read', method='GET')
def get_inspection(business_name):
  try:
    result=collection.find(business_name)
  except ValidationError as ve:
    print('Error with find')
    abort(404)
  return json.loads(json.dumps(result, indent=4, default=json_util.default))

## create
@route('/create', method='POST')
def create_inspection(document): 
  try:
    result=collection.save(document)
  except ValidationError as ve:
    print('Error with insert')
    abort(404)
  return json.loads(json.dumps(result, indent=4, default=json_util.default))

## update
@route('/update', method='PUT')
def update_inspection(id, update):
  try:
    result=collection.update(id, update)
  except ValidationError as ve:
    print('Error with update')
    abort(404)
  return json.loads(json.dumps(result, indent=4, default=json_util.default))

## delete
@route('/delete', method='DELETE')
def delete_inspection(id):
  try:
    result=collection.delete_one(id)
  except ValidationError as ve:
    print('Error with delete')
    abort(404)
  return jsonify( { 'result': True } )

## oldest inspection
@route('/inspectionsDue', method='GET')
def topfive():
  try:
    result=list(collection.find().sort('date',pymongo.DESCENDING).count(5))
  except ValidationError as ve:
    print('Error with delete')
    abort(404)
  return json.loads(json.dumps(result, indent=4, default=json_util.default))
  
if __name__ == '__main__':
  #app.run(debug=True)
  run(host='localhost', port=8080)