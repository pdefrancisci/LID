#!/usr/local/bin/python3
import cv2 as cv
import numpy as np
import urllib.request
import datetime
from PIL import Image
import os
import time as t

time = datetime.datetime.now().strftime("%I:%M%p%B%d%Y")
pp_name = "/home/pjd/LID/pp/"+time+".gif"
urllib.request.urlretrieve("https://radar.weather.gov/ridge/standard/KBUF_0.gif",
	pp_name)

image = Image.open(pp_name)

gifToPngOut = "/home/pjd/LID/pp/"+time+".png"
image.save(gifToPngOut)
os.remove(pp_name)

image = cv.imread(gifToPngOut,cv.IMREAD_GRAYSCALE)
#1 bit conversion
#_, image = cv.threshold(image, 128, 255, cv.THRESH_BINARY)
os.remove(gifToPngOut)

proc = "/home/pjd/LID/proc/"+time+".bmp"
image = image[30:520, 0:600]
#pretty sure this should be a 7 bit greyscale reduction
image = (image // 32) * 32
cv.imwrite(proc,image)

for fname in os.listdir("/home/pjd/LID/proc/"):
	fpath = os.path.join("/home/pjd/LID/proc/",fname)
	if os.path.isfile(fpath):
		file_age = t.time() - os.path.getmtime(fpath)
		if file_age > 3600:
			os.remove(fpath)
