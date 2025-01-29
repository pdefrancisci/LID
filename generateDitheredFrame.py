#!/usr/local/bin/python3
import numpy as np
import urllib.request
import datetime
from PIL import Image, ImageOps
import os
import time as t

#the nws includes key info, we can crop this out
TOP_CROP=100
BOT_CROP=30

#python imaging libraries don't like .gif, just get it into memory
time = datetime.datetime.now().strftime("%I:%M%p%B%d%Y")
pp_name = "/home/pjd/LID/pp/"+time+".gif"
urllib.request.urlretrieve("https://radar.weather.gov/ridge/standard/KBUF_0.gif",
    pp_name)
image = Image.open(pp_name)
gifToPngOut = "/home/pjd/LID/pp/"+time+".png"
image.save(gifToPngOut)
os.remove(pp_name)

#convert to 8 bit greyscale
image = image.convert("L")

#a description of bayer dithering would be out of scope
#I started with a typical 2x2 dither bayer dither
#And just tweaked it until I liked how it looked

#if this looks really blown out, here's the key from some experiements I did
#1: the darkest part of the storm, if this number goes up much further you can't read text
#2: The middle part of the storm, having this in between 1 and 3 gives us a gradient
#3: THe lightest part of the storm, but also the great lakes haha
#4:The lightest part of the map, this was 0, but I set it to 50 to make the outline stronger  
#so now storms, rather than being a big grey mess, storms still *usually* show outlines!
bayer=np.array([[100, 140], [225, 50]])

#so, apply fs dithering, then show it
#crop 
image = image.crop([0,TOP_CROP,image.width,image.height-BOT_CROP])
#scale
image = ImageOps.scale(image,4/3)
#take the bayer array and make it fit the dimensions of the image
h,w=image.size
bmh,bmw=bayer.shape
bayer_tile=np.tile(bayer,(h//(bmh-1),w//(bmw-1)))
#crop the tile to fit
bayer_tile=bayer_tile[:w,:h]
#apply the dithering
image=(image>bayer_tile).astype(np.uint8)*255
image=Image.fromarray(image)
image=image.convert("1")
image.save("/home/pjd/LID/dith/"+time+".bmp")

#we need to clean up the old images as well
for fname in os.listdir("/home/pjd/LID/dith/"):
    fpath = os.path.join("/home/pjd/LID/dith/",fname)
    if os.path.isfile(fpath):
        file_age = t.time() - os.path.getmtime(fpath)
        if file_age > 3600:
            os.remove(fpath)
