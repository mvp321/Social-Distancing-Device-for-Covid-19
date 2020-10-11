

from __future__ import print_function
from imutils.object_detection import non_max_suppression
from imutils import paths 
import cv2
import numpy as np
from imutils.video import VideoStream
import imutils
import time 
import sys
import serial
import RPi.GPIO as GPIO

print("Start")
bluetooth = serial.Serial("/dev/rfcomm0", 38400)
print("Connected")
bluetooth.flushInput()
GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)
GPIO.setup(24, GPIO.IN)
usingPiCamera = True
# Set initial frame size.
frameSize = (320, 240)
cascPath = "haarcascade_frontalface_default.xml"
faceCascade = cv2.CascadeClassifier(cascPath) 
# Initialize mutithreading the video stream.
vs = VideoStream(src=0, usePiCamera=usingPiCamera, resolution=frameSize,
        framerate=32).start()
# Allow the camera to warm up.
time.sleep(2.0)
timeCheck = time.time()

while True:
    # Get the next frame.
    motion = GPIO.input(24)
    frame = vs.read()

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    faces = faceCascade.detectMultiScale(
        gray,
        scaleFactor=1.1,
        minNeighbors=5,
        minSize=(30, 30),
        flags=cv2.CASCADE_SCALE_IMAGE
    )
    if motion == 0 and len(faces)==0:
        print ("No Person Detected")
        bluetooth.write(str.encode(str(0)))
    else:
        print ("Person Detected")
        bluetooth.write(str.encode(str(1)))
   # else:
    #    bluetooth.write(str.encode(str(0)))

    # Draw a rectangle around the faces
#    for (x, y, w, h) in faces:
#        cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 2)

    # Display the resulting frame
    cv2.imshow('Video', frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# When everything is done, release the capture
video_capture.release()
cv2.destroyAllWindows()


