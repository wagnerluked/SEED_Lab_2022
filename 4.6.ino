//The purpose of this code is to find the transfer function of a motor
//by applying a step input and recording the angular velocity.

#include "Arduino.h"

//Variables

int ENCA = 2;                // Encoder Channel A (Yellow) on int pin 2
int ENCB = 4;              // Encoder Channel B (White) on pin X 
//#define MAXPULSES 800         // Encoder pulses per revolution

// varaibles for serial communication
String InputString = ""; // a string to hold incoming data
bool StringComplete = false;

//Constants
const float Pi = 3.14159;

//Motor pins
int enable = 4;
int motor1Volt = 9;
int motor2Volt = 10;
int motor1Dir = 7;
int motor2Dir = 8;
int statusFlag = 12;

//Motor Contol
//Motor volt pwm command
int m1vCommand = 0;
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

void setup() {
  Serial.begin(115200);
  // reserve 200 bytes for the inputString:
  InputString.reserve(200);
  Serial.println("Ready!"); // Let anyone on the other end of the serial line know that Arduino is ready
  stopFlag = false;

  pinMode(ENCA, INPUT_PULLUP);
  pinMode(ENCB, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENCA), encoderISR, RISING);
  

  //Configures pins
  pinMode(enable, OUTPUT);
  pinMode(motor1Dir, OUTPUT);
  pinMode(motor2Dir, OUTPUT);
  pinMode(motor1Volt, OUTPUT);
  pinMode(motor2Volt, OUTPUT);
  pinMode(statusFlag, INPUT);

  digitalWrite(enable, HIGH);
  //Low = forward, high = reverse;
  digitalWrite(motor1Dir, LOW);
  analogWrite(motor1Volt, m1vCommand);
  analogWrite(motor2Volt, m2vCommand);

}

void loop() {
//Clears flag for MATLAB serial reading
  if (StringComplete) {
    StringComplete = false;
  }

//Flag lets code run once until next reset
  if(stopFlag == false) {
    //Get values from 1 - 2 seconds
    if(millis() >= 1000 && millis() <= 2000){
      //Sets sample rate of 5ms
      if(millis() >= currentTime + period){ 
        currentTime = millis();

        //Sets pwm on motor
        m1vCommand = 125;
        analogWrite(motor1Volt, m1vCommand);
  
        //Encoder resolution:  64 CPR
        counts = currentPos - lastPos;
        angularVel = (1000 / period) * (2 * PI / 800) * counts;

    Serial.println(counts);
        //Time
        //Serial.print(currentTime);
        //Serial.print("\t");
        
        //Motor voltage Command
        //Serial.print(m1vCommand);
        //Serial.print("\t");
        
        //Angular Velocity
        //Serial.print(angularVel);
        //Serial.println(" ");
      }
    }else if(millis() > 2000) {
      Serial.println("Finished");
      stopFlag = true;
    }
  }
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */

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

// ISR Function
void encoderISR(){
  int val = digitalRead(ENCB);
Serial.println("here");
  // If Channel B is HIGH, turning CW
  if (val > 0){
    counts++; // Forward
  }else{
    counts--; //Reverse
  }
}
