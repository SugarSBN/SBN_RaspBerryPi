
import cv2
import subprocess
import face_recognition
import numpy as np
import socket
import sys
import datetime

rtmp = 'rtmp://123.56.12.187/hls/room1'
 

cap = cv2.VideoCapture(0)
size = (int(cap.get(cv2.CAP_PROP_FRAME_WIDTH)), int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT)))
sizeStr = str(size[0]) + 'x' + str(size[1])
 
command = ['ffmpeg',
    '-y', '-an',
    '-f', 'rawvideo',
    '-vcodec','rawvideo',
    '-pix_fmt', 'bgr24',
    '-s', sizeStr,
    '-r', '25',
    '-i', '-',
    '-c:v', 'libx264',
    '-pix_fmt', 'yuv420p',
    '-preset', 'ultrafast',
    '-f', 'flv',
    rtmp]
 
pipe = subprocess.Popen(command
    , shell=False
    , stdin=subprocess.PIPE
)


	
server_addr = ('localhost', 2300)
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
count = 0
while cap.isOpened():
	success,frame = cap.read()
	process_this_frame = True
	count = count+1
	if count == 20:
		count = 0
	if count != 0:
		datet = str(datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S"))
		font = cv2.FONT_HERSHEY_SIMPLEX  # 定义字体
		imgzi = cv2.putText(frame, datet, (50, 50), font, 1.2, (255, 255, 255), 2)
		cv2.imshow('Video', frame)
		pipe.stdin.write(frame.tostring())
		continue
	if success:
		small_frame = cv2.resize(frame, (0, 0), fx = 0.25, fy = 0.25)
		rgb_small_frame = small_frame[:,:,::-1]
		if process_this_frame:
			face_locations = face_recognition.face_locations(rgb_small_frame)
			face_names = []
			face_names.append('Unknown')
		process_this_frame = not process_this_frame
		for (top, right, bottom, left), name in zip(face_locations, face_names):
			top *= 4
			right *= 4
			left *= 4
			bottom *= 4
			
			
			centre = str((top + bottom) / 2) + ',' + str((left + right) / 2) + ',' + str(bottom - top)
			try:
			    s.connect(server_addr)
			    print("Connected to {:s}".format(repr(server_addr)))
			except:
			    print("rua!")
			try:
				#s.connect(server_addr)
				s.send(centre.encode())
			except AttributeError as ae:
				print("Error creating the socket: {}".format(ae))
			except socket.error as se:
				print("Exception on socket: {}".format(se))
			finally:
				print("Closing socket")
				#s.close()
			    
			cv2.rectangle(frame, (left, top), (right, bottom), (0, 0, 255), 2)
		datet = str(datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S"))
		font = cv2.FONT_HERSHEY_SIMPLEX  # 定义字体
		imgzi = cv2.putText(frame, datet, (50, 50), font, 1.2, (255, 255, 255), 2)
		
		cv2.imshow('Video', frame)
		if cv2.waitKey(1) & 0xFF == ord('q'):
			break    
		pipe.stdin.write(frame.tostring())
 
cap.release()
pipe.terminate()

