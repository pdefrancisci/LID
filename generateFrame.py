#!/usr/local/bin/python3
import cv2 as cv
import numpy as np
import urllib.request
import datetime
from PIL import Image
import os

time = datetime.datetime.now().strftime("%I:%M%p%B%d%Y")
pp_name = "/home/pjd/inkplate/pp/"+time+".gif"
urllib.request.urlretrieve("https://radar.weather.gov/ridge/standard/KBUF_0.gif",
	pp_name)

image = Image.open(pp_name)

gifToPngOut = "/home/pjd/inkplate/pp/"+time+".png"
image.save(gifToPngOut)
os.remove(pp_name)

image = cv.imread(gifToPngOut,0)
os.remove(gifToPngOut)

proc = "/home/pjd/inkplate/proc/"+time+".png"
image = image[30:520, 0:600]
shades=8
gray_levels=256//shades
image=(image // gray_levels) * gray_levels

cv.imwrite(proc,image)
