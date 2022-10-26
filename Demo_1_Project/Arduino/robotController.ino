//The purpose of this code is to find the transfer function of a two motor
//by applying a step input

#include <Encoder.h>
#include "Arduino.h"

//Variables

int m1ENCA = 3;              // Encoder Channel A (Yellow) on int pin 2
int m1ENCB = 6;              // Encoder Channel B (White) on pin X 
int m2ENCA = 2;                   
int m2ENCB = 5;
Encoder enc1(m1ENCA, m1ENCB);
Encoder enc2(m2ENCA, m2ENCB);

// varaibles for serial communication
String InputString = ""; // a string to hold incoming data
bool StringComplete = false;

//Constants
const float WHEEL_RADIUS = 2.94; //inches
const float WHEEL_DISTANCE = 14; //inches
const float BATTERY_VOLTAGE = 7.4; //volts
const int COUNTS_PER_REV = 3200;

//Motor pins
int enable = 4;
int motor1Volt = 9;
int motor2Volt = 10;
int motor1Dir = 7;
int motor2Dir = 8;
int statusFlag = 12;

//Motor Contol
//Motor volt pwm command
int m1PWM = 0;
int m2PWM = 0;
float m1Volt = 0;
float m2Volt = 0;

float rho = 0;
float phi = 0;
float rho_dot = 0;
float phi_dot = 0;
float forwardVolt = 0;
float angularVolt = 0;

float phiTarget = 0;
float rhoTarget = 0;
float phi_dotTarget = 0;
float rho_dotTarget = 0;

float phiFudge = 0;

//Encoder and calculations
//motor 1
int m1DeltaPos = 0;
int m1LastPos = 0;
volatile long m1Counts = 0;
float m1Theta = 0;
float m1Theta_dot = 0;
//motor 2
int m2DeltaPos = 0;
int m2LastPos = 0;
volatile long m2Counts = 0;
float m2Theta = 0;
float m2Theta_dot = 0;

//TIME
  unsigned long Ts = 0;
  unsigned long Tc = 1000;
  int runTime = 1000 + 100000;
  int stopFlag = true;
  int period = 5;
  unsigned long currentTime = 0;

//Phi PID Controller
  float phiKp = 6;//4.5;    //0.13; // v/rad
  float phiKd = 0;
  float phiKi = 0.0001;//0.0001;
  float phiD = 0;
  float phiI = 0;
  float phiError = 0;
  float phiErrorPast = 0;

//Phi_dot PI Controller
  float phi_dotKp = 3; // v/rad //3.5
  float phi_dotKi = 0.0000000000000001;      //0.00025
  float phi_dotI = 0;
  float phi_dotError = 0;
  int phi_dotIncrement = 1;

//Rho_dot PI Controller
  float rho_dotKp = 1.6; // v/rad //1
  float rho_dotKi = 0.00000000000000001;    //0.00003
  float rho_dotI = 0;
  float rho_dotError = 0;
  int rho_dotIncrement = 1;

void setup() {
  Serial.begin(115200);
  // reserve 200 bytes for the inputString:
  InputString.reserve(200);
  Serial.println("Ready!"); // Let anyone on the other end of the serial line know that Arduino is ready

  pinMode(m1ENCA, INPUT_PULLUP);
  pinMode(m1ENCB, INPUT);
  //attachInterrupt(digitalPinToInterrupt(ENCA), encoderISR, RISING);
  
  //Configures pins
  pinMode(enable, OUTPUT);
  pinMode(motor1Dir, OUTPUT);
  pinMode(motor2Dir, OUTPUT);
  pinMode(motor1Volt, OUTPUT);
  pinMode(motor2Volt, OUTPUT);
  pinMode(statusFlag, INPUT);

  digitalWrite(enable, HIGH);
  //Low = forward, high = reverse;
  digitalWrite(motor1Dir, HIGH);
  digitalWrite(motor2Dir, LOW);
  analogWrite(motor1Volt, m1PWM);
  analogWrite(motor2Volt, m2PWM);
}

void loop() {
//Clears flag for MATLAB serial reading
  if (StringComplete) {
    StringComplete = false;
  }

  //Delays start by 1 second and sets run time
  if(millis() >= 1000 && millis() - currentTime >= period){
    currentTime = millis();

    //Test values <----Change these
    phiTarget =  -2*PI;
    if (phiTarget != 0) {
       phiFudge = (phiTarget + (2 * PI)) * 0.022; //0.022 for pi, 0.025 for pi/2, 0.0 for 2PI
       phiTarget = phiTarget - phiFudge;
    }
   
    
    rhoTarget = 12*3;
    
    //Values eventually provided by camera
    //Semi-Circle of radius 24 inches in 10 seconds
    //phi_dotTarget = 0; //  radians/sec
    //rho_dotTarget = 6; //  inches/sec

  //*********************************************************************//
  // START    PHI PID CONTROLLER   OUTER LOOP
  //*********************************************************************//
    //Calculates phiError   
    phiError = phiTarget - phi; //rads

//    //Calculates derivative term
//    if(Ts > 0) {
//      phiD = (phiError - phiErrorPast) / Ts;
//      phiErrorPast = phiError; 
//    } else phiD = 0;
    //Calculates integal term
    phiI = phiI + (Ts * phiError);

    if(abs(phiError) <= 0.01) phiI = 0;

       
    //Calculates desired angular velocity input
    phi_dotTarget = (phiKp * phiError) + (phiKi * phiI); //

    //m1Volt = (phiKp * phiError) + (phiKi * phiI); //
    //m2Volt = -1 * ((phiKp * phiError) + (phiKi * phiI)); //

    
  //*********************************************************************//
  // END   PHI PID CONTROLLER    OUTER LOOP
  //*********************************************************************//

  //*********************************************************************//
  // START    PHI_DOT PI CONTROLLER   INNER LOOP
  //*********************************************************************//
    //Increases phi_dot slowly to eliminate jumping
    if(phi_dotIncrement <= 20) {
        phi_dotTarget = phi_dotTarget * 0.05 * phi_dotIncrement;
        phi_dotIncrement++;
    }

    //Calculates phi_dotError
    phi_dotError = phi_dotTarget - phi_dot; //rads/sec

    //Gets integral value
    //phi_dotI = phi_dotI + (Ts * phi_dotError);
    //Calculates angular velocity input
    angularVolt = (phi_dotKp * phi_dotError); // + (phi_dotKi * phi_dotI); //volts
    
    //Anti-windup for integral
//    if (abs(angularVolt) > BATTERY_VOLTAGE) {
//      angularVolt = sgn(angularVolt)* BATTERY_VOLTAGE;
//      phi_dotError = sgn(phi_dotError)* min(BATTERY_VOLTAGE / phi_dotKp, abs(phi_dotError));
//      phi_dotI = (angularVolt - (phi_dotKp * phi_dotError)) / phi_dotKi;
//    }
  //*********************************************************************//
  // END   PHI_DOT PI CONTROLLER    INNER LOOP
  //*********************************************************************//


  //*********************************************************************//
  // START    RHO_DOT PI CONTROLLER   INNER LOOP
  //*********************************************************************//   
    //Sets a forward velocity once the phi has reached its target
    //rho_dotTarget = 12;
     if(stopFlag == false) {
         rho_dotTarget = 12;
        ////Increases rho_dot slowly to eliminate jumping
        if(rho_dotIncrement <= 20) {
            rho_dotTarget = rho_dotTarget * 0.05 * rho_dotIncrement;
            rho_dotIncrement++;
        }
    

    if(rho >= 0.99 * rhoTarget) rho_dotTarget = 0;
    

    //Calculates rho_dotError
      rho_dotError = rho_dotTarget - rho_dot; //rads/sec

    //Calculates integral value
    //rho_dotI = rho_dotI + (Ts * rho_dotError);
    //Calculates forward velocity input
    forwardVolt = (rho_dotKp * rho_dotError); //+ (rho_dotKi * rho_dotI); //volts
     }
    //Anti-windup for integral
//    if (abs(forwardVolt) > BATTERY_VOLTAGE) {
//      forwardVolt = sgn(forwardVolt)* BATTERY_VOLTAGE;
//      rho_dotError = sgn(rho_dotError)* min(BATTERY_VOLTAGE / rho_dotKp, abs(rho_dotError));
//      //rho_dotI = (forwardVolt - (rho_dotKp * rho_dotError)) / rho_dotKi;
//    }
  //*********************************************************************//
  // END   RHO_DOT PI CONTROLLER    INNER LOOP
  //*********************************************************************//


  //*********************************************************************//
  // START    IMPLMENTING CALULATED VOLTAGES
  //*********************************************************************//
    //Sets voltage for each motor
    m1Volt = (forwardVolt + angularVolt) / 2;
    m2Volt = ((forwardVolt - angularVolt) / 2);

    

    //Sets direction based on sign of voltage
    //motor 1
    if(m1Volt < 0) digitalWrite(motor1Dir, LOW);
      else digitalWrite(motor1Dir, HIGH);
    //motor2
    if(m2Volt < 0) digitalWrite(motor2Dir, HIGH);
      else digitalWrite(motor2Dir, LOW);

    //Sets pwm on motors
    
    m1PWM = (abs(m1Volt) / BATTERY_VOLTAGE) * 125;
    m2PWM = (abs(m2Volt) / BATTERY_VOLTAGE) * 125;
    if(m1PWM > 125) m1PWM = 125;
    if(m2PWM > 125) m2PWM = 125;
    
    analogWrite(motor1Volt, m1PWM);
    analogWrite(motor2Volt, m2PWM);

    if(abs(phiError) <= 0.15 && stopFlag == true) {
      analogWrite(motor1Volt, 0);
      analogWrite(motor2Volt, 0);
      phi = phiTarget;
      phiI = 0;
      rho = 0.5;
      stopFlag = false;
      delay(500);
    }
    
  //*********************************************************************//
  // END    IMPLMENTING CALULATED VOLTAGES
  //*********************************************************************//


  //*********************************************************************//
  // START    CALCULATE RHO_DOT AND PHI_DOT
  //*********************************************************************//
    //Gets current position
    m1Counts = -1 * enc1.read(); //counts
    m2Counts = enc2.read(); //counts

    //Updates times
      Ts = millis() - Tc;
      Tc = millis();
    
    //Calculates theta and theta_dot for motor1
      m1DeltaPos = m1Counts - m1LastPos;
      m1Theta = (2 * PI / COUNTS_PER_REV) * m1DeltaPos; //rads
      m1Theta_dot = (1000 / Ts) * m1Theta; //rads/sec
      m1LastPos = m1Counts;
  
      //Calculates theta and theta_dot for motor2
      m2DeltaPos = m2Counts - m2LastPos;
      m2Theta = (2 * PI / COUNTS_PER_REV) * m2DeltaPos; //rads
      m2Theta_dot = (1000 / Ts) * m2Theta; //rads/sec
      m2LastPos = m2Counts;

      //Calculates total rho and total phi
      rho = rho + WHEEL_RADIUS * ((m1Theta + m2Theta) / 2); //inches
      phi = phi + (WHEEL_RADIUS * ((m1Theta - m2Theta) / WHEEL_DISTANCE)); //rads
   
      //Calculates rho_dot and phi_dot
      rho_dot = WHEEL_RADIUS * ((m1Theta_dot + m2Theta_dot) / 2); // inches/sec                     
      phi_dot = (WHEEL_RADIUS * ((m1Theta_dot - m2Theta_dot) / WHEEL_DISTANCE)); // rad/sec
  //*********************************************************************//
  // END    CALCULATE RHO_DOT AND PHI_DOT
  //*********************************************************************//


  //*********************************************************************//
  // START    PRINT STATMENTS FOR MATLAB
  //*********************************************************************//   

    Serial.print(Tc - 1000);
    Serial.print("\t");
//   Serial.print(m1Volt);
//   Serial.print("\t");
//   Serial.print(m2Volt);
//   Serial.print("\t");
//   Serial.print(phi_dotTarget);
//   Serial.print("\t");
//   Serial.print(phi_dotError);
//   Serial.print("\t");
//   Serial.print(rho_dotTarget);
//   Serial.print("\t");
//   Serial.print(rho_dotError);
//   Serial.print("\t");
//   Serial.print("\t");
//   
//   /unts);
    //Serial.print("\t");
    Serial.print(m1PWM);
   Serial.print("\t");
   Serial.print(m2PWM);
   Serial.print("\t");
  Serial.print("\t");
   Serial.print(m1Counts);
   Serial.print("\t");
   Serial.print(m2Counts);
   Serial.print("\t");
   Serial.print("\t");
  // Serial.print(forwardVolt);
 //  Serial.print("\t");
  // Serial.print(angularVolt);
//
//   Serial.print(phiTarget);
//   Serial.print("\t");
//   Serial.print(phi);
//  Serial.print("\t");
//   Serial.print(phiError);
//  Serial.print("\t");
//   Serial.print("\t");
//     Serial.print(phi);
//     Serial.print("\t");
//     Serial.print(phi_dotTarget);
//     Serial.print("\t");
//     Serial.print(phi_dot);
//     Serial.print("\t");
   
     //Serial.print("\t");
//   Serial.print(rho_dotTarget);
//   Serial.print("\t");
//     Serial.print(rho_dot);
//     Serial.print("\t");
//   Serial.print(rho_dotError);
//  Serial.print("\t");  
  Serial.print(rho);
   Serial.println(" ");
  }
  //*********************************************************************//
  // END    PRINT STATMENTS FOR MATLAB
  //*********************************************************************//   
  
  //Ends test after the run time expires
//  if(millis() > runTime && stopFlag == false) {
//    Serial.println("Finished");
//    stopFlag = true;
//    analogWrite(motor1Volt, 0);
//    analogWrite(motor2Volt, 0);
//  }
}

//*********************************************************************//
// FUNCTIONS
//*********************************************************************//   

//Configures serial port to read values into MATLAB
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

//Function to return the sign of the values
int sgn(int val) {
  if (val < 0) return -1;
  if (val == 0) return 0;
  return 1;
}