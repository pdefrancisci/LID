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
			dith_img = dith_img.crop((0,0,800,600))
			output_path = output_dir / f"{file.stem}.bmp"
			dith_img.save(output_path, format="BMP")
			os.remove(file)
