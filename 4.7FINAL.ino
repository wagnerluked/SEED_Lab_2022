//The purpose of this code is to turn a wheel to a given quadrant using a PI controller

#include <Encoder.h>
#include "Arduino.h"
#include <Wire.h>

//Variables

// varaibles for serial communication
String InputString = ""; // a string to hold incoming data
bool StringComplete = false;

int ENCA = 3;                // Encoder Channel A (Yellow) on int pin 2
int ENCB = 2;              // Encoder Channel B (White) on pin X 
Encoder enc(ENCA, ENCB);

//Constants
const float Pi = 3.14159;
#define SLAVE_ADDRESS 0x04

//Motor pins
int enable = 4;
int motor1Volt = 9;
int motor2Volt = 10;
int motor1Dir = 7;
int motor2Dir = 8;
int statusFlag = 12;

//Motor Contol
//Motor volt pwm command
float m1vCommand = 0;
int m2vCommand = 0;

//Encoder and calculations
int currentPos = 0;
int lastPos = 0;
volatile int counts = 0;
float angularVel = 0;

//Time
int period = 5;
unsigned long currentTime = 0;
int stopFlag;
bool newValue = true;

//Controller
  float Kp = 1.25; // v/rad
  float Ki = 0.00008;
  float I = 0;
  float error = 0;
  float output = 0;
  float umax = 7.5;
  int Ts = 0;
  int Tc = 0;
  int refVal = 0;
  int currentVal = 0;
  int quad = 0;
  float radError = 0;
  bool epicFlag = true;


  float rad = 0;

void setup() {
  Serial.begin(115200);


  // reserve 200 bytes for the inputString:
  InputString.reserve(200);
  Serial.println("Ready!"); // Let anyone on the other end of the serial line know that Arduino is ready
  stopFlag = false;
  Wire.begin(SLAVE_ADDRESS);  // For I2C

  // define callbacks for i2c communication
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);
  
  //Configures pins
  pinMode(ENCA, INPUT_PULLUP);
  pinMode(ENCB, INPUT);
  pinMode(enable, OUTPUT);
  pinMode(motor1Dir, OUTPUT);
  pinMode(motor2Dir, OUTPUT);
  pinMode(motor1Volt, OUTPUT);
  pinMode(motor2Volt, OUTPUT);
  pinMode(statusFlag, INPUT);
  pinMode(13, OUTPUT);  // For I2C

  digitalWrite(enable, HIGH);
  //Low = forward, high = reverse;
  digitalWrite(motor1Dir, LOW);
  analogWrite(motor1Volt, m1vCommand);
}

void loop() {
  //Clears flag for MATLAB serial reading
  if (StringComplete) {
    StringComplete = false;
  }

  if(millis() > 1000){
  
  //read in value here*********************************************************************************
  //quad = 4;
  
  //Sets reference value given quadrant
  if(quad == 1) {
    if(epicFlag == true) {
      refVal = 0;
    } else {
      refVal = 3200;
      
    } 
  }
  else if(quad == 2){
    refVal = 800;
    epicFlag = true;
    }
  else if(quad == 3) {
    refVal = 1600;
    epicFlag = false;
  }
  else if(quad == 4) {
    refVal = 2400;
    epicFlag = false;
  }
  else refVal = 0;
  
  //gets current position of motor
  currentVal = enc.read();
  
  //Calculates error
  error = refVal - currentVal;
  radError = error * 2 * PI / 3200;
  //Provides error bounds
  if(abs(error) <= 25){
    
    radError = 0;
    if(currentVal > 3000) {
      enc.write(0);
      epicFlag = true;
    }
      
      
      //Serial.print("This is the current counts after reset ");
      //Serial.println(currentVal);

   
  }
 
  //Gets integral value
  I = I + (Ts * radError);
  //Calculates output error in volts
  output = (Kp * radError) + (Ki * I);

//Serial.print(error);
//  Serial.print("\t");

//Keeps output confined to voltage range of battery
  if (abs(output)> umax) {
    output = sgn(output)* umax;
    error = sgn(radError)* min(umax / Kp, abs(radError));
    I = (output - (Kp * error)) / Ki;
  }
  
  //Sets direction based on if error is pos or neg
  if(sgn(error) == -1) {
    digitalWrite(motor1Dir, HIGH);
  }
  else {
    digitalWrite(motor1Dir, LOW);
  }

  //turns volts into pwm amount
  m1vCommand = abs(output) / 7.6 * 256;


//  Serial.print(error);
//  Serial.print("\t");
//  Serial.print(output);
//  Serial.print("\t");
//  Serial.println(m1vCommand);

  
  //Writes PWM value to motor
  analogWrite(motor1Volt, m1vCommand);
  
  //Updates times
  Ts = millis() - Tc;
  Tc = millis();

  rad = currentVal * 2 * PI / 3200;
//  Serial.print(millis());
//  Serial.print("\t");
//  Serial.print(rad);
//  Serial.println(" ");

  if(millis() > 7000) {
      //Serial.println("Finished");
      //stopFlag = true;
    }
  
  //delay(5);
  }
}

//Function to return the sign of the values
int sgn(int val) {
  if (val < 0) return -1;
  if (val == 0) return 0;
  return 1;
}


void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    InputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      StringComplete = true;
    }
  }
}

void receiveData(int byteCount){
  
  Serial.print("data received: ");
  while(Wire.available()) {
    quad = Wire.read(); //quad 1,2,3,4
    
    Serial.print(quad);
    Serial.print(' ');
  }
  Serial.println(' ');  
}

// callback for sending datamm
void sendData(){
  
  Wire.write(quad);
}
