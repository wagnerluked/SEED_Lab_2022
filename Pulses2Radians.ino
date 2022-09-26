#define ENCA 2                // Encoder Channel A (Yellow) on int pin 2
#define ENCB 4                // Encoder Channel B (White) on pin X 
#define MAXPULSES 800         // Encoder pulses per revolution
#define PI 3.14               // Constant PI

volatile int pulses = 0;

//double nowRads = 0;
//double prevRads = 0;
//double maxRads = 2*PI;

// Timing variables
int printerval = 500;
long prevTime = 0;
long nowTime = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Wheel Radians");
  pinMode(ENCA, INPUT_PULLUP);
  pinMode(ENCB, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENCA), encoderISR, RISING);

}

void loop() {
  float radians = 0;
  nowTime = millis();

  // Prints radians every printerval
  if(nowTime - prevTime > printerval){
    prevTime = nowTime;

    if(pulses > 0 && MAXPULSES > pulses){
      radians = (float)2*pulses/MAXPULSES;
    }else{
      radians = radians - 2*pulses/MAXPULSES;
    }
    
    Serial.print("Orientation: ");
    Serial.print(radians);
    Serial.println(" * PI");
    
  }
  

}

// ISR Function
void encoderISR(){
  int val = digitalRead(ENCB);

  // If Channel B is HIGH, turning CW
  if (val > 0){
    pulses++; // Forward
  }else{
    pulses--; //Reverse
  }
}
