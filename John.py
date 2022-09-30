import cv2 as cv
import numpy as np
from picamera.array import PiRGBArray
from picamera import PiCamera
import time
import cv2.aruco as aruco
import smbus2
import board
import adafruit_character_lcd.character_lcd_rgb_i2c as character_lcd
# for RPI version 1, use "bus = smbus.SMBus(0)"
bus = smbus2.SMBus(1)

# This is the address we setup in the Arduino Program
address = 0x04

# Modify this if you have a different sized Character LCD
lcd_columns = 16
lcd_rows = 2
# Initialise I2C bus.
i2c = board.I2C()  # uses board.SCL and board.SDA
# Initialise the LCD class
lcd = character_lcd.Character_LCD_RGB_I2C(i2c, lcd_columns, lcd_rows)


def writeNumber(value):
    bus.write_byte(address, value)
    #bus.write_byte_data(address, 0, value)
    return -1

def readNumber():
    number = bus.read_byte(address)
    #number = bus.read_byte_data(address, 1)
    return number

#need to:
#    -calibration step, turn camera on let auto white blance settle to a const, save, auto turned off
quadrant = 0

cap = cv.VideoCapture(0)
if not cap.isOpened():
    print("cannot open camera")
    exit()

while True:
    try:
        #capture frame by frame
        ret, frame = cap.read()
        
        #if frame is read correctly red is True
        if not ret:
            print("Can't receive frame (stream end?). Exiting...")
            break
        
        #frame operations
        gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
        aruco_dict = aruco.Dictionary_get(aruco.DICT_6X6_250)
        parameters = aruco.DetectorParameters_create()
        corners, ids, rejected = aruco.detectMarkers(gray, aruco_dict, parameters=parameters)
                 
        #marker position calculation
        avgx = (corners[0][0][0][0] + corners[0][0][1][0] + corners[0][0][2][0] + corners[0][0][3][0]) / 4
        avgy = (corners[0][0][0][1] + corners[0][0][1][1] + corners[0][0][2][1] + corners[0][0][3][1]) / 4
        center = [avgx, avgy]
        
        if ((center[0] < 340) & (center[1] < 240)):
            quadrant = 1
        elif ((center[0] > 340) & (center[1] < 240)):        
            quadrant = 2
        elif center > [340, 240]:
            quadrant = 3
        else:
            quadrant = 4
        
        #print(quadrant)
        gray = aruco.drawDetectedMarkers(gray, corners, ids)
        
        #display resulting frame
        cv.imshow('frame', gray) 
        if cv.waitKey(1) == ord('q'):
            break
        
        
        writeNumber(quadrant)
        #print ("RPI: Hi Arduino, I sent you %s" % quadrant)
        # sleep one second
        #time.sleep(1)

        number = readNumber()
        print ("Arduino: Hey RPI, I received a digit %s" % number)
        
        lcd.clear()
        # Set LCD color to red
        lcd.color = [50, 0, 50]
        #time.sleep(1)
        # Print two line message
        lcd.message = "sent: "+ str(quadrant) +"\ngot: " + str(number)
        
    except: #allows program to run when marker is not present
        if cv.waitKey(1) == ord('q'):
            break
        continue
    
#release capture when task is complete
cap.release()
cv.destroyAllWindows()