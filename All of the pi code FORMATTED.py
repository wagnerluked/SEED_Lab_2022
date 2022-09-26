# All of the pi code ------------------------------------------------------------------------------------
# John Buchholz
# EENG 350 Exercise 2


# ----------------------------------- Part 1 -----------------------------------------
# This code will establish 2-way communication between the Arduino and Pi using I2C. 
# The Pi will request an integer value from the user and send it to the Arduino.
# The Arduino will add 5 to the number and then send it back to Pi.
# ------------------------------------------------------------------------------------
# Hardware requirements:
# Raspberry Pi, Arduino and wires 3 wires to connect them together for the I2C link.
# ------------------------------------------------------------------------------------

import smbus2
import time
# for RPI version 1, use "bus = smbus.SMBus(0)"
bus = smbus2.SMBus(1)

# This is the address we setup in the Arduino Program
address = 0x04

def writeNumber(value):
    bus.write_byte(address, value)
    #bus.write_byte_data(address, 0, value)
    return -1

def readNumber():
    number = bus.read_byte(address)
    #number = bus.read_byte_data(address, 1)
    return number

while True:
    
    var = input("Enter 1 - 9: ")
    var = int(var)
    
    if not var:
            continue
        
    writeNumber(var)
    print ("RPI: Hi Arduino, I sent you %s" % var)
    # sleep one second
    time.sleep(1)

    number = readNumber()
    print ("Arduino: Hey RPI, I received a digit %s" % number)



# ----------------------------------- Part 2 -----------------------------------------
# This code will establish 2-way communication between the Arduino and Pi using I2C. 
# The Pi asks for both a value and an offset, and executes a bus.write_byte_data.
# The Arduino saves the value to a different variable depending on the offset.
# The Pi then asks for an offset to be read, and executes a bus.read_byte_data.
# If the i2c read is for offset 0, 5 is added to the number that was saved,
# but if the offset is 1, 10 is added to the number.
# ------------------------------------------------------------------------------------
# Hardware requirements:
# Raspberry Pi, Arduino and wires 3 wires to connect them together for the I2C link.
# LCD display
# ------------------------------------------------------------------------------------

import smbus2
import time
# for RPI version 1, use "bus = smbus.SMBus(0)"
bus = smbus2.SMBus(1)

# This is the address we setup in the Arduino Program
address = 0x04

print ("Off set value of 0: 5")
print ("Off set value of 1: 10")
offSet = input("Enter an off set value of 0 or 1: ")

offSet = int(offSet)

def writeNumber(value):
    #bus.write_byte(address, value)
    bus.write_byte_data(address, offSet, value)
    return -1

def readNumber():
    #number = bus.read_byte(address)
    number = bus.read_byte_data(address, 1)
    return number

while True:
    
    var = input("Enter 1 - 9: ")
    var = int(var)

    if not var:
            continue
        
    writeNumber(var)
    print ("RPI: Hi Arduino, I sent you %s" % var)
    # sleep one second
    time.sleep(1)

    number = readNumber()
    print ("Arduino: Hey RPI, I received a digit %s" % number)



# ----------------------------------- Part 3 -----------------------------------------
# This code will establish 2-way communication between the Arduino and Pi using I2C. 
# The Pi asks for both a value and an offset, and executes a bus.write_byte_data.
# The Arduino saves the value to a different variable depending on the offset.
# The Pi then asks for an offset to be read, and executes a bus.read_byte_data.
# If the i2c read is for offset 0, 5 is added to the number that was saved,
# but if the offset is 1, 10 is added to the number.
# For this code the LCD will display the data being sent to and received from the
# Arduino on the LCD.
# ------------------------------------------------------------------------------------
# Hardware requirements:
# Raspberry Pi, Arduino and wires 3 wires to connect them together for the I2C link.
# LCD display
# ------------------------------------------------------------------------------------

import smbus
import time
import board
import adafruit_character_lcd.character_lcd_rgb_i2c as character_lcd


# Modify this if you have a different sized Character LCD
lcd_columns = 16
lcd_rows = 2
# Initialise I2C bus.
i2c = board.I2C()  # uses board.SCL and board.SDA
# Initialise the LCD class
lcd = character_lcd.Character_LCD_RGB_I2C(i2c, lcd_columns, lcd_rows)


# for RPI version 1, use “bus = smbus.SMBus(0)”
bus = smbus.SMBus(1)

# This is the address we setup in the Arduino Program
address = 0x04


print ("Off set value of 0: 5")
print ("Off set value of 1: 10")
offSet = input("Enter an off set value of 0 or 1: ")

offSet = int(offSet)

def writeNumber(value):
    #bus.write_byte(address, value)
    bus.write_byte_data(address, offSet, value)
    return -1

def readNumber():
    #number = bus.read_byte(address)
    number = bus.read_byte_data(address, 1)
    return number

while True:
    
    var = input("Enter 1 - 9: ")
    var = int(var)

    if not var:
            continue
        
    writeNumber(var)
    print ("RPI: Hi Arduino, I sent you %s" % var)
    # sleep one second
    time.sleep(1)

    number = readNumber()
    print ("Arduino: Hey RPI, I received a digit %s" % number)
    
    lcd.clear()
    # Set LCD color to red
    lcd.color = [50, 0, 50]
    time.sleep(1)
    # Print two line message
    lcd.message = "sent: "+ str(var) +"\ngot: " + str(number)



# ----------------------------------- Part 5 -----------------------------------------
# This code will establish 2-way communication between the Arduino and Pi using I2C.
# We will use write_i2c_block_data and read_i2c_block_data to send multiple bytes
# back and forth between the Arduino and Pi. The Pi will store a user-defined string.
# Then convert the string to integers and send it to the Arduino. 
# The Arduino will reverse the order of the data and send it back to the Pi,
# which then converts and prints the reversed string. 
# ------------------------------------------------------------------------------------
# Hardware requirements:
# Raspberry Pi, Arduino and wires 3 wires to connect them together for the I2C link.
# LCD display and potentiometer.
# ------------------------------------------------------------------------------------

import smbus2
import time
# for RPI version 1, use “bus = smbus.SMBus(0)”
bus = smbus2.SMBus(1)

# This is the address we setup in the Arduino Program
address = 0x04

# Function to convert string to ascii
def convert_to_ascii(text):
    converted = []
    for b in text:
        converted.append(ord(b))
    return converted
# Function to convert ascii to char
def convert_to_string(var):
    converted = []
    for b in var:
        converted.append(chr(b))
    return converted
# Function to convert char to string 
def list_to_string(thing):
    str1 = ""
    return (str1.join(thing))


def writeText(value):
    bus.write_i2c_block_data(address, 0, value)
    return -1

def readText(length):
    block = bus.read_i2c_block_data(address, 0, length)
    return block

while True:
    
    data1 = input("Enter a string: ")
    data = convert_to_ascii(data1)
    length = len(data)
    if not data:
            continue
        
    writeText(data)
    print ("RPI: Hi Arduino, I sent you %s" % data1)
    print ("Which converts to in ascii: %s" %data)
    
    # sleep one second
    time.sleep(1)
    block = readText(length)
    convertedBlock = (list_to_string(convert_to_string(block)))
    print ("Arduino: Hey RPI, I am sending a reversed block: %s" % convertedBlock)



# ----------------------------------- Part 6 -----------------------------------------
# This code will establish 2-way communication between the Arduino and Pi using I2C.
# The Arduino will read the voltage across the potentiometer and send this
# information to the Pi, and have the Pi will display the voltage on the LCD screen.
# ------------------------------------------------------------------------------------
# Hardware requirements:
# Raspberry Pi, Arduino and wires 3 wires to connect them together for the I2C link.
# ------------------------------------------------------------------------------------

import smbus2
import time
import board
import adafruit_character_lcd.character_lcd_rgb_i2c as character_lcd

# Modify this if you have a different sized Character LCD
lcd_columns = 16
lcd_rows = 1
# Initialise I2C bus.
i2c = board.I2C()  # uses board.SCL and board.SDA
# Initialise the LCD class
lcd = character_lcd.Character_LCD_RGB_I2C(i2c, lcd_columns, lcd_rows)

# for RPI version 1, use “bus = smbus.SMBus(0)”
bus = smbus2.SMBus(1)

# This is the address we setup in the Arduino Program
address = 0x04
data = 0
# encoding = 'utf-8'

startTime = time.time()
while True:
    block = bus.read_i2c_block_data(address, 0, 2)
    
    num = block[0] + (block[1] * 256)

    print(num)
    
    lcd.clear()
    # Set LCD color to red
    lcd.color = [50, 0, 50]
    time.sleep(1)
    # Print 1 line message
    lcd.message = "Pot Value: "+str(num)
    time.sleep(1.0 - ((time.time() - startTime) % 1.0))



# ----------------------------------- Part 7 -----------------------------------------
# This code will establish 2-way communication between the Arduino and Pi using I2C.
# We will use write_i2c_block_data and read_i2c_block_data to send multiple bytes
# back and forth between the Arduino and Pi. The Pi will store a user-defined string.
# Then convert the string to integers and send it to the Arduino. 
# The Arduino will reverse the order of the data and send it back to the Pi,
# which then converts and prints the reversed string. 
# This code is identical to part 5s code but with the exception of error handling.
# The program doesn't quit when you remove the SDA pin.
# When there is an error the Pi and LCD screen print "I2C Error".
# ------------------------------------------------------------------------------------
# Hardware requirements:
# Raspberry Pi, Arduino and wires 3 wires to connect them together for the I2C link.
# LCD display and potentiometer.
# ------------------------------------------------------------------------------------

import smbus2
import time
import board
import adafruit_character_lcd.character_lcd_rgb_i2c as character_lcd

# Modify this if you have a different sized Character LCD
lcd_columns = 16
lcd_rows = 1
# Initialise I2C bus.
i2c = board.I2C()  # uses board.SCL and board.SDA
# Initialise the LCD class
lcd = character_lcd.Character_LCD_RGB_I2C(i2c, lcd_columns, lcd_rows)
# for RPI version 1, use “bus = smbus.SMBus(0)”
bus = smbus2.SMBus(1)

# This is the address we setup in the Arduino Program
address = 0x04

def convert_to_ascii(text):
    converted = []
    for b in text:
        converted.append(ord(b))
    return converted

def convert_to_string(var):
    converted = []
    for b in var:
        converted.append(chr(b))
    return converted

def list_to_string(thing):
    str1 = ""
    return (str1.join(thing))


def writeText(value):
    bus.write_i2c_block_data(address, 0, value)
    return -1

def readText(length):
    block = bus.read_i2c_block_data(address, 0, length)
    return block

while True:
    try:
        
        data1 = input("Enter a string: ")
        data = convert_to_ascii(data1)
        length = len(data)
        if not data:
                continue
            
        writeText(data)
        print ("RPI: Hi Arduino, I sent you %s" % data1)
        print ("Which converts to in ascii: %s" %data)
        
        # sleep one second
        time.sleep(1)
        block = readText(length)
        convertedBlock = (list_to_string(convert_to_string(block)))
        print ("Arduino: Hey RPI, I am sending a reversed block: %s" % convertedBlock)
        lcd.clear()
        # Set LCD color to purple
        lcd.color = [50, 0, 50]
        time.sleep(1)
        # Print 1 line message
        lcd.message = "MSG  Successful!"
    except:
        lcd.clear()
        # Set LCD color to purple
        lcd.color = [50, 0, 50]
        time.sleep(1)
        # Print 1 line message
        lcd.message = "I2C Error"



# ----------------------------------- Part 8 -----------------------------------------
# This code will establish 2-way communication between the Arduino and Pi using Serial.
# The Pi will request an integer value from the user and send it to the Arduino.
# The Arduino will add 5 to the number and then send it back to Pi.
# ------------------------------------------------------------------------------------
# Hardware requirements:
# Raspberry and Arduino.
# ------------------------------------------------------------------------------------

import serial
import time

#Set address
ser = serial.Serial('/dev/ttyACM0', 115200)
#Wait for connection to complete
time.sleep(3)

#Function to read serial
def ReadfromArduino():
    while (ser.in_waiting > 0):
        try:
            line = ser.readline().decode('utf-8').rstrip()
            print("serial output : ", line)
        except:
            print("Communication Error")
#How to send a string

value = input("Enter 1 – 9: ")
value = bytes(value, 'utf-8')

#Remeber to encode the string to bytes
ser.write(value)

# wait for Arduino to set up response
time.sleep(2)
ReadfromArduino()

print("Done")