// All of the Arduino code ------------------------------------------------------------------------------------
// John Buchholz
// EENG 350 Exercise 2


// ----------------------------------- Part 1 -----------------------------------------
// This code will establish 2-way communication between the Arduino and Pi using I2C. 
// The Pi will request an integer value from the user and send it to the Arduino.
// The Arduino will add 5 to the number and then send it back to Pi.
// ------------------------------------------------------------------------------------
// Hardware requirements:
// Raspberry Pi, Arduino and wires 3 wires to connect them together for the I2C link.
// ------------------------------------------------------------------------------------

#include <Wire.h>

#define SLAVE_ADDRESS 0x04
int number = 0;
int state = 0;

void setup() {
pinMode(13, OUTPUT);
Serial.begin(115200); // start serial for output
// initialize i2c as slave
Wire.begin(SLAVE_ADDRESS);

// define callbacks for i2c communication
Wire.onReceive(receiveData);
Wire.onRequest(sendData);

Serial.println("Ready!");
}

void loop() {
delay(100);
}

// callback for received data
void receiveData(int byteCount){
  
  Serial.print("data received: ");
  while(Wire.available()) {
    number = Wire.read();
    
    // This will add 5 to the number sent by the Pi
    number= number + 5;
    Serial.print(number);
    Serial.print(' ');
  }
  Serial.println(' ');

// callback for sending data
void sendData(){
Wire.write(number);
}



// ----------------------------------- Part 2 and 3 -----------------------------------
// For part 2:
// This code will establish 2-way communication between the Arduino and Pi using I2C. 
// The Pi asks for both a value and an offset, and executes a bus.write_byte_data.
// The Arduino saves the value to a different variable depending on the offset.
// The Pi then asks for an offset to be read, and executes a bus.read_byte_data.
// If the i2c read is for offset 0, 5 is added to the number that was saved,
// but if the offset is 1, 10 is added to the number.
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// For part 3:
// This will be the same block of code used for both part 2 and 3.
// For this code the LCD will display the data being sent to and received from the
// Arduino on the LCD.
// ------------------------------------------------------------------------------------
// Hardware requirements:
// Raspberry Pi, Arduino and wires 3 wires to connect them together for the I2C link.
// LCD display
// ------------------------------------------------------------------------------------

#include <Wire.h>

#define SLAVE_ADDRESS 0x04
int number = 0;
int state = 0;

int data[32] = {0};

void setup() {
  pinMode(13, OUTPUT);
Serial.begin(115200); // start serial for output
// initialize i2c as slave
Wire.begin(SLAVE_ADDRESS);

// define callbacks for i2c communication
Wire.onReceive(receiveData);
Wire.onRequest(sendData);

Serial.println("Ready!");
}

void loop() {
  delay(100);
}

// callback for received data
void receiveData(int byteCount){
  int i = 0;

  
  Serial.print("data received: ");
  while (Wire.available()){

    // i is the length and data is the message
    data[i] = Wire.read();
    
    Serial.print(data[i]);
    Serial.print(' ');
    i++;
  }
  Serial.println(' ');
  i--;

  // if offset = 0 then run this statement
  if (data[0] == 0){
    data[i] = data[i] + 5;
  }

  
  // if offset = 1 then run this statement
  if (data[0] == 1){
    data[i] = data[i] + 10;
  }
  
}

// callback for sending data
void sendData(){
  Wire.write(data[1]);
}



// ----------------------------------- Part 5 and 7 -----------------------------------
// For part 5: 
// This code will establish 2-way communication between the Arduino and Pi using I2C.
// We will use write_i2c_block_data and read_i2c_block_data to send multiple bytes
// back and forth between the Arduino and Pi. The Pi will store a user-defined string.
// Then convert the string to integers and send it to the Arduino. 
// The Arduino will reverse the order of the data and send it back to the Pi,
// which then converts and prints the reversed string. 
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// For part 7:
// We will use the same code in this part as in part 5.
// ------------------------------------------------------------------------------------
// Hardware requirements:
// Raspberry Pi, Arduino and wires 3 wires to connect them together for the I2C link.
// LCD display and potentiometer.
// ------------------------------------------------------------------------------------

#include <Wire.h>

#define SLAVE_ADDRESS 0x04
char number = 0;
char data[32] = {0};
char reverseData[32] = {0}; 
int i = 0;
int n = 0;

void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(115200); // start serial for output
  // initialize i2c as slave
  Wire.begin(SLAVE_ADDRESS);

  // define callbacks for i2c communication
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);

  Serial.println("Ready!");
}

void loop() {
  delay(100);
}

// callback for received data
void receiveData(int byteCount){

  Serial.print("data received: "); 
  while(Wire.available()) {
    number = Wire.read();
    Serial.print(number);
    data[i] = number;
    i++;
  }
  i--;
    // This variable called reverseData will be where we store the reveresed string.
    // Conting back from the end of the string to the start and then returning it.
    if (i != 0) {
      n = i;
      for (i; i >= 0; i--) {
        reverseData[i] = data[n - i];
      }

      Serial.println(' ');
      Serial.print("This is the reversed string: ");
      i = 0;

      for (i; i <= n; i++) {
        Serial.print(reverseData[i]);
      }
    
      Serial.println(' ');
      i = 0;
    }

}

// callback for sending data
void sendData(){
  int j = 0;

  Serial.println(' ');
  Serial.print("--> Sending data:");

  for(j; j < n; j++){ 
    Serial.print(reverseData[j]);
    Wire.write(reverseData[j]);
  }
  Serial.println(' ');
}



// ----------------------------------- Part 6 -----------------------------------------
// This code will establish 2-way communication between the Arduino and Pi using I2C.
// The Arduino will read the voltage across the potentiometer and send this
// information to the Pi, and have the Pi will display the voltage on the LCD screen.
// ------------------------------------------------------------------------------------
// Hardware requirements:
// Raspberry Pi, Arduino and wires 3 wires to connect them together for the I2C link.
// ------------------------------------------------------------------------------------

#include <Wire.h>

#define SLAVE_ADDRESS 0x04
int number = 0;
int sensorPin = A0;
int sensorValue = 0;

void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(115200); // start serial for output
  // initialize i2c as slave
  Wire.begin(SLAVE_ADDRESS);

  // define callbacks for i2c communication

  Wire.onRequest(sendData);
  Serial.println("Ready!");
}

void loop() {
  delay(100);
}

// callback for sending data
void sendData(){
  
  // This will take the pot value and seperate it into two parts.
  // The first part is the least sig bit and then the other is the Most sig bit.
  // Since the Pi can only recive single bytes and the pot goes up to 1000+
  // we will need to seperate the pot value into two bits and then send them individually 
  // We can do this with a bit shift and a mask as seen below.
  sensorValue = analogRead(sensorPin);
  uint8_t LSB = (sensorValue &0x00ff);
  uint8_t MSB = (sensorValue >> 8);

  Serial.print(sensorValue);
  Serial.println(" ");
  
  Wire.write(LSB);
  Wire.write(MSB);
  Serial.println(" ");
}



// ----------------------------------- Part 8 -----------------------------------------
// This code will establish 2-way communication between the Arduino and Pi using Serial.
// The Pi will request an integer value from the user and send it to the Arduino.
// The Arduino will add 5 to the number and then send it back to Pi.
// ------------------------------------------------------------------------------------
// Hardware requirements:
// Raspberry and Arduino.
// ------------------------------------------------------------------------------------

String data;
bool DataRead;
int data1 = 0;

void setup() {
    Serial.begin(115200);
}

void loop() {

    if (DataRead) {
        Serial.print("You sent me: ");
        Serial.println(data1);
        DataRead = false;
    }
}

void serialEvent(){

// We are going to put all the code here...
    if (Serial.available() > 0) {
      // Data is the string we get from the pi.
        data = Serial.readStringUntil('\n');
        // Since the data comes in as a string we need to convert it into an int before we can add 5 to it.
        data1 = data.toInt();
        data1 = data1 + 5;
        DataRead = true;
    }
    Serial.flush();
}