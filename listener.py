#!/usr/bin/env python3
import serial
import time
import json
from pprint import pprint

##############################################################

import psycopg2
from configparser import ConfigParser
from config import load_config

def load_config(filename='database.ini', section='postgresql'):
    parser = ConfigParser()
    parser.read(filename)

    # get section, default to postgresql
    config = {}
    if parser.has_section(section):
        params = parser.items(section)
        for param in params:
            config[param[0]] = param[1]
    else:
        raise Exception('Section {0} not found in the {1} file'.format(section, filename))

    return config

def connect(config):
    """ Connect to the PostgreSQL database server """
    try:
        # connecting to the PostgreSQL server
        with psycopg2.connect(**config) as conn:
            print('Connected to the PostgreSQL server.')
            return conn
    except (psycopg2.DatabaseError, Exception) as error:
        print(error)

#############################################################

class Record(object):
    def __init__(self, j):
        self.__dict__ = json.loads(j)

def get_records():
    conn = psycopg2.connect(
            host="localhost",
            database="sensor_data",
            user="postgres",
            password="duster2000")
    ser = serial.Serial('/dev/ttyACM0',9600, timeout=1.0)
    time.sleep(3)
    ser.reset_input_buffer();
    print("Serial ok")
    try:
        while True:
            time.sleep(0.01)
            if ser.in_waiting > 0:
                line = ser.readline().decode('utf-8')
                if line[0] == '{':
                    rec = Record(line)
                    pprint(vars(rec))

    except KeyboardInterrupt:
        print("Close Serial")
        ser.close()

###############################################################

if __name__ == '__main__':
    get_records()
    #config = load_config()
    #print(config)
