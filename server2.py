#!/usr/bin/env python3
import serial
import time
import json
from flask import Flask
import os
import glob
from listener import get_record
from pprint import pprint
import jsonify

# Define the folder containing images
IMAGE_FOLDER = '/home/pjd/inkplate/proc'
PORT = 8008

SERIAL_PORT = '/dev/ttyACM0'  # Adjust as needed
BAUD_RATE = 9600
TIMEOUT = 2

# Flask constructor takes the name of 
# current module (__name__) as argument.
app = Flask(__name__)

# The route() function of the Flask class is a decorator, 
# which tells the application which URL should call 
# the associated function.
@app.route('/sensors', methods=['GET'])
def check_sensors():
    rec = get_record()
    return rec
    
    
# The route() function of the Flask class is a decorator, 
# which tells the application which URL should call 
# the associated function.
@app.route('/radar')
def radar():
    return None
    
    
def get_newest_image(folder):
    #search for images
    image_files=glob.glob(os.path.join(folder,'*.bmp'))
    if not image_files:
        print('ahhh!')
        return None
    new_image = max(image_files,key=os.path.getmtime)
    return new_image

# main driver function
if __name__ == '__main__':
    # run() method of Flask class runs the application 
    # on the local development server.
    app.run()