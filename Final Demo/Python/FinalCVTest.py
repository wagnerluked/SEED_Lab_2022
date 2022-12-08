#Jamie Armstrong
#EENG350 Final Computer Vision Subsystem
#This program contains the complete computer vision algorithm used in our project. It builds on the angle/distance detection used in previous demos,
#and contains additional code necessary for the final demo. In particular, the additions made in this code allow the camera to distinguish between multiple
#markers present at the same time, and perform angle/distance operations on only a single marker at a time. It also sneds movement instruction to our
#Arduino contrl code via I2C, and handles the final demo path traversing.

import adafruit_character_lcd.character_lcd_rgb_i2c as character_lcd
from picamera.array import PiRGBArray
from picamera import PiCamera
import cv2.aruco as aruco
import smbus2
import numpy as np
import board
import busio
import time
import cv2

# global Variables
bus = smbus2.SMBus(1)
address = 0x04
desiredPos = 0
offSet = 3

# This is used to write a value to the arduino via I2C
def writeText(value):
    bus.write_byte_data(address, offSet, value)
    return -1

#initialize the camera, grab reference to raw camera capture
camera = PiCamera()

width, height = (640, 480)
camera.resolution = (width, height)
camera.framerate = 30
rawCapture = PiRGBArray(camera, size=camera.resolution)

#camera to warmup
time.sleep(0.1)

#set ISO to desired value
camera.iso = 100

#wait for automatic gain control to settle
time.sleep(2)

#now fix values
camera.shutter_speed = camera.exposure_speed
camera.exposure_mode = 'off'
g = camera.awb_gains
camera.awb_mode = 'off'
camera.awb_gains = g

#camera parameters used later in determining marker pose, and subsequently distance/angle
K = np.array([[426.7, 0, 332], [0, 412, 272.3], [0, 0, 1]])
distCoeffs = np.array([-.010, .045, -.007, .003, -.163])

#dictionary and parameters
aruco_dict = aruco.Dictionary_get(aruco.DICT_4X4_250)
parameters = aruco.DetectorParameters_create()
MARKER_LENGTH = 5  # cm

#variables used in final demo
id_list = np.zeros((6,1)) #two numpy arrays of fixed sizes, easier to perform operations on than standard tvecs and ids arrays
tvec_list = np.zeros((6,3))
marker = 1 #initially searching for marker of id 1
rho = 50
decFlag = False
phi_cnts = 0

while (marker <= 7): #marker "countdown" governs entire program
    try:
        #capture frames from the camera
        for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
            image = frame.array
            
            id_list = np.zeros((6,1)) #initialize all id and tvec values to 0 at start of each loop, information stays up to date when marker leaves frame
            tvec_list = np.zeros((6,3))
            
            id_list[0] = 1 
           
            #clear stream
            rawCapture.truncate(0)

            gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

            #detect Markers
            corners, ids, rejectedImgPoints = aruco.detectMarkers(gray, aruco_dict, parameters=parameters)

            #estimate marker pose
            rvecs, tvecs, _ = aruco.estimatePoseSingleMarkers(corners, markerLength=MARKER_LENGTH, cameraMatrix=K, distCoeffs=distCoeffs)

            if ids is None: #check for markers in frame
                pass
                print("no markers, target marker: ", marker)
            
            else:               
                for i in range(6): #for loop populates id_list and tvec_list with whatever is in ids and tvecs
                    for j in range(len(ids)): #ids and tvecs are sorted the same way relative to markers so ids_list and tvecs_list filled at same time
                        if (i == ids[j]):
                            id_list[i] = ids[j]
                            tvec_list[i] = tvecs[j]
                            
                print("marker found")
                
                targetTVec = tvec_list[marker] #establishes tvec corresponding to target marker only, this is later used in angle/distance calculation

                print("target marker: ",marker)
                if (id_list[marker] == marker): #if we see target marker in frame
                    decFlag = True
                    #perform normal angle/distance calculations
                    phi = (-1) * np.arctan2(targetTVec[0], targetTVec[2]) 
                    rho = targetTVec[2] #Z distance in translation vector as distance from camera in cm
                    print("rho: ", rho)
                    print("phi: ", np.rad2deg(phi), "\n")
                    phi_cnts = int(np.rad2deg(phi) * 3200 / 360) #phi into an integer representing encoder counts for Arduino code
                    #the following sends movement commands to Arduino controller via I2C, offSet value determiens state of Arduino FSM by changing a single bit
                    if(phi_cnts < 0): #negative angle
                        offSet = 0
                        offSet = int(offSet)
                        phi_cnts = abs(phi_cnts)
                                                      
                    else: #positive angle
                        offSet =  1
                        offSet = int(offSet)
                        phi_cnts = abs(phi_cnts)
                        
                else: #otherwise, if we do NOT see target marker, offSet=3, corresponding to searching state 
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
                 
            cv2.imshow('frame', gray) #display video stream in window
            if cv2.waitKey(1) == ord('q'):
                break #close window
    except:
        continue

cv2.destroyAllWindows() #end of program, occurs when all markers have been found and reached



