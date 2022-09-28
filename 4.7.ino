#include <Encoder.h>

//The purpose of this code is to find the transfer function of a motor
//by applying a step input and recording the angular velocity.

#include "Arduino.h"

//Variables

int ENCA = 3;                // Encoder Channel A (Yellow) on int pin 2
int ENCB = 2;              // Encoder Channel B (White) on pin X 
Encoder enc(ENCA, ENCB);
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
bool newValue = true;

//Controller
  float Kp = 0.253604;
  float Ki = 0.008536;
  float I = 0;

  int error = 0;
  int errorPast = 0;
  int Ts = 0;
  int Tc = 0;
  int refVal = 0;
  int currentVal = 0;
  float output = 0;

void setup() {
  Serial.begin(115200);

  pinMode(ENCA, INPUT_PULLUP);
  pinMode(ENCB, INPUT);
  
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
}

void loop() {

  //Reference value -> comes from Jam, set to 400 counts right now to move wheel 180 deg
  refVal = 400
  //gets current value of motor
  currentVal = enc.read();
 //Calculates error
  error = refVal - currentVal;
  //Gets intesgral value
  I = I + Ts * error;
  //Calculates output error in volts (I think)
  output = Kp * error + Ki * I;

//need to implment
//If abs(u)>umax,
//u := sgn(u)*umax
//e = sgn(e)*min(umax / Kp, abs(e))
//I := (u-Kp*e-Kd*D)/Ki

  
  //turns volts into pwm amount by manipulating this formula V=(pwmVal/257)Vin, Vin = 7.6V
  m1vCommand = output / 7.6 * 256;
  //Writes PWM value to motor
  analogWrite(motor1Volt, m1vCommand);
  //Updates times
  Ts = millis() - Tc;
  Tc = millis();
}
