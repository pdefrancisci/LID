#!/usr/local/bin/python3
import numpy as np
import urllib.request
import datetime
from PIL import Image, ImageOps
import os
import time as t
from pathlib import Path

directory = Path("/home/pjd/inkplate/")
input_dir = directory/"images"
output_dir = directory/"dith_img"

input_dir.mkdir(exist_ok=True)
output_dir.mkdir(exist_ok=True)

for file in input_dir.iterdir():
	if file.suffix.lower() in [".png", ".jpg", ".jpeg", ".bmp"]:
		with Image.open(file) as img:
			dith_img = img.convert("1",dither=Image.FLOYDSTEINBERG)
			ratio = dith_img.height / dith_img.width
			new_height = int(ratio*800)
			dith_img = dith_img.resize((800,new_height), Image.LANCZOS)
			dith_img = dith_img.crop((0,0,800,600))
			if new_height > 600:
				h_start = (new_height-600)/2
				dith_img = dith_img.crop((0,h_start,800,600+h_start))
			output_path = output_dir / f"{file.stem}.bmp"
			dith_img.save(output_path, format="BMP")
			os.remove(file)
