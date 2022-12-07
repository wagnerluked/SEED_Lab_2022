#final demo aruco hierarchy test

#Jamie Armstrong
#EENG350 Demo 2 Computer Vision Subsystem
#Angle and Distance Detection

import adafruit_character_lcd.character_lcd_rgb_i2c as character_lcd
from picamera.array import PiRGBArray
from picamera import PiCamera
import cv2.aruco as aruco
import smbus2
import numpy as np
import pickle
import board
import busio
import time
import cv2

# Global Variables
bus = smbus2.SMBus(1)
address = 0x04
desiredPos = 0
lcd_columns = 16
lcd_rows = 2
offSet = 3

# This is used to write a value to the arduino via I2C
def writeText(value):
    bus.write_byte_data(address, offSet, value)
    return -1

# initialize the camera, grab reference to raw camera capture
camera = PiCamera()

width, height = (640, 480)
camera.resolution = (width, height)
camera.framerate = 30
rawCapture = PiRGBArray(camera, size=camera.resolution)

# Allow the camera to warmup
time.sleep(0.1)

# Set ISO to desired value
camera.iso = 100

# Wait for automatic gain control to settle
time.sleep(2)

# Now fix values
camera.shutter_speed = camera.exposure_speed
camera.exposure_mode = 'off'
g = camera.awb_gains
camera.awb_mode = 'off'
camera.awb_gains = g

# Camera Parameters
objects = []
with (open("camera_params.p", "rb")) as openfile:
    while True:
        try:
            objects.append(pickle.load(openfile))
        except EOFError:
            break

K = objects[0]['M']
distCoeffs = objects[0]['coefs_dist']

# Create dictionary and parameters
aruco_dict = aruco.Dictionary_get(aruco.DICT_4X4_250)
parameters = aruco.DetectorParameters_create()
MARKER_LENGTH = 5  # cm

#print(K)
#print(distCoeffs)

id_list = np.zeros((6,1))
tvec_list = np.zeros((6,3))
marker = 5
rho = 50
decFlag = False
phi_cnts = 0

while (marker >= 0):
    #print("stuff")
    try:
        # capture frames from the camera
        for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
            # grab the raw NumPy array representing the image, then initialize the timestamp
            # and occupied/unoccupied text
            image = frame.array
            
            id_list = np.zeros((6,1))
            tvec_list = np.zeros((6,3))
            
            id_list[0] = 1
            
            

            # clear the stream in pre
            rawCapture.truncate(0)

            gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

            # Detect Markers
            corners, ids, rejectedImgPoints = aruco.detectMarkers(gray, aruco_dict, parameters=parameters)

            # Get Pose
            rvecs, tvecs, _ = aruco.estimatePoseSingleMarkers(corners, markerLength=MARKER_LENGTH, cameraMatrix=K, distCoeffs=distCoeffs)

            if ids is None:
                pass
                print("no markers, target marker: ", marker)
            
            else:
                #print(marker)
                marker_id = ids[0, 0]
                marker_corners = corners[0]
                rvec = rvecs[0]
                tvec = tvecs[0]
                #print("marker found")
                
                for i in range(6):
                    for j in range(len(ids)):
                        if (i == ids[j]):
                            id_list[i] = ids[j]
                            tvec_list[i] = tvecs[j]
                            
                print("marker found")
                            
#                for k in range(6):
#                    for l in range(len(tvecs)):
#                        if (k == l):
#                            tvec_list[k] = tvecs[l]

                            
                #print(id_list)
                #print(tvec_list)
                #print(marker)
                #print(tvec_list[marker - 1])
                
                targetTVec = tvec_list[marker]
                #print(targetTVec)
                
                #largestID = max(ids[0])
                #print("largestID: ", largestID)
                #print("targetID: ", marker)
                #length = len(ids) - 1
                #print("length: ", length)
                #print(tvec)
                #print(tvecs)
                #print(ids)
                print("target marker: ",marker)
                if (id_list[marker] == marker): #if we see target 
                    decFlag = True
                    #perform normal angle/distance calculations
                    phi = (-1) * np.arctan2(targetTVec[0], targetTVec[2])
                    rho = targetTVec[2]
                    print("rho: ", rho)
                    print("phi: ", np.rad2deg(phi), "\n")
                    phi_cnts = int(np.rad2deg(phi) * 3200 / 360)
                    
                    if(phi_cnts < 0): #negative angle
                        offSet = 0
                        offSet = int(offSet)
                        phi_cnts = abs(phi_cnts)
                                                      
                    else:
                        offSet =  1
                        offSet = int(offSet)
                        phi_cnts = abs(phi_cnts)
                        
                else: #otherwise, if we do NOT see target marker
                    offSet = 3
                    offSet = int(offSet)
                    print("Target marker not found\n")
                                   
                if ((rho < 30) & (decFlag == True)): #if we arrive at the desired marker
                    offSet = 2
                    offSet = int(offSet)
                    marker -= 1
                    decFlag = False
                    if(marker == -1):
                        marker = 0
                         
                try:                       #Added for error handling
                    writeText(phi_cnts)
                    
                except:                    #Added for error handling
                    continue
                    
            gray = aruco.drawDetectedMarkers(gray, corners, ids)
                 
            cv2.imshow('frame', gray) 
            if cv2.waitKey(1) == ord('q'):
                break
    except:
        continue

cv2.destroyAllWindows()



