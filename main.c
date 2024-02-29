/*
  Property of Plasmagear Inc. 2023-09-11
  Company Contact: info@plasmagear.ca
  Developer Contact: Christopher@plasmagear.ca
  software developed by Christopher Pederson
  product designation: AirClenzTM

  ---DEV NOTES---
  look at the company software protocol to see installation proceedure
  Version History: 1.0
  ---------------

*/
#include <SharpIR.h>
#include <EEPROM.h>
#define IR A0 // define signal pin
#define model 1080 // used 1080 because model GP2Y0A21YK0F is used
SharpIR SharpIR(IR, model);
int calibrationInput = 6;// pin value for calibration switch
int indicatorLED = 2;//pin value indicator LED 
int calibrationAdress = 0;// we will only use the first byte of our available EEPROM, this value will not be iterated
int calibrationValue = -1;// calibration value that is set using the stored EEPROM value
int currentValue = -1;// value that constantly monitors the IR range sensor during normal operations
int fans = 3;// controller pin for the MOSFET controlling the fans


void setup() {
 Serial.begin(9600); // standard baud rate for range sensor
 pinMode(calibrationInput, INPUT);// setting pin states
 pinMode(indicatorLED, OUTPUT);
 pinMode(fans, OUTPUT);

 EEPROM.write(calibrationAdress, -1);// null value for calibration distance 

 delay(1000);//sensors can output bad values imidiatly after power up 

 if (calibrationState()){//checks to see if calibration switch is on, if it is it will run the calibration protocol 
    calibrate();
 }

 calibrationValue = EEPROM.read(calibrationAdress);// sets the our calibrationValue to the saved calibration point

 calibrationCheck(calibrationValue);// checks the stored calibration value, if the value is deemed bad this will initiate a closed loop

}

void loop() {
  currentValue = getValue(); // sets current value to the average of 30 readings
  if ((calibrationValue + 5) > currentValue || (calibrationValue - 5) < currentValue){//gives a tollerance of 5cm to the calibration value
    digitalWrite(fans, LOW);
  }
  else{
    digitalWrite(fans, HIGH);// turns the fans on if the current value is outside of tollerance 
  }
}

int readDistance(){// returns the distance read by the IR range sensor
  unsigned long startTime=millis();
  int dis=SharpIR.distance();
  return dis;
}

bool calibrationState(){//checks calibration switch state
  if (digitalRead(calibrationInput) == 1){//switch is on
    return true;
  }
  else{//switch is off
    return false;
  }
}

int calibrate(){
  int sum = 0;//sum total of all 30 readings
  int avg = 0;//average of all 30 values

  delay(60000);

  for(int i = 0; i < 30; i++){//takes 30 readings from the IR sensor and adds them to sum 
    sum += readDistance();
    delay(1000);
  }

  avg = sum/30;//averages the values
  EEPROM.write(calibrationAdress, avg);//saves the average to the designated space in EEPROM
}

void calibrationCheck(int val){
  if (val < 10 || val > 90){// checksvalue limits to ensure a good value
    badCalibration();//LED indicator
  }
  else{
    goodCalibration();//LED indicator 
  }
}

void badCalibration(){//rapidly blinks forever until the device is reset to show a bad calibration result
  while (true){
  digitalWrite(indicatorLED, HIGH);
  delay(100);
  digitalWrite(indicatorLED, LOW);
  delay(100);
  }
}

void goodCalibration(){
    for(int i = 0; i < 3; i++){//slow blink 3 times to show a good calibration result
      digitalWrite(indicatorLED, HIGH);
      delay(1000);
      digitalWrite(indicatorLED, LOW);
      delay(1000);
    }
}

int getReading(){
  int sum = 0;//sum total of all 30 readings
  int avg = 0;//average of all 30 values

  for(int i = 0; i < 30; i++){//takes 30 readings from the IR sensor and adds them to sum 
    sum += readDistance();
    delay(1000);
  }

  avg = sum/30;//averages the values 

  return avg;
}
