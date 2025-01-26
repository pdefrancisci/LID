#!/usr/bin/env python3
import serial
import time
import json
from pprint import pprint

#############################################################

class Record(object):
    def __init__(self, j):
        self.__dict__ = json.loads(j)

def get_records():
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
