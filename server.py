import http.server
import socketserver
import os
import glob

# Define the folder containing images
IMAGE_FOLDER = '/home/pjd/inkplate/proc'
PORT = 8008

class ImageRequestHandler(http.server.SimpleHTTPRequestHandler):
	def do_GET(self):
		#get newest image
		new_image=get_newest_image(IMAGE_FOLDER)
		if new_image:
			#serve!
			self.send_response(200)
			self.send_header('Content-type','image/png')
			self.end_headers()
			with open(new_image,'rb') as file:
				self.wfile.write(file.read())
		else:
			self.send_response(404)
			self.end_headers()
			self.wfile.write(b"No image in"+IMAGE_FOLDER)

def get_newest_image(folder):
	#search for images
	image_files=glob.glob(os.path.join(folder,'*.png'))
	if not image_files:
		print('ahhh!')
		return None
	new_image = max(image_files,key=os.path.getmtime)
	return new_image

def run_server():
	with socketserver.TCPServer(("",PORT), ImageRequestHandler) as httpd:
		print(f"Serving at http://localhost:{PORT}")
		httpd.serve_forever()

if __name__ == "__main__":
	run_server()
