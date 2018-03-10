#include <DS1307RTC.h>
#include <Time.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotor = AFMS.getMotor(1); //Port M1

float dt = 1; // Loop time - must be one second to keep from feeding multiple times in a given second
const int button = 2; // Digital Pin 2 for push button
volatile uint32_t lastInterrupt = 0; // Last time interrupt was called
const int rotarySwitch = A0; // Analog input zero for rotary switch input
const uint16_t voltageBitDivision = 1023/2; //Integer bit value of each increment of the rotary switch ouput voltage
const int maxSteps = 200; //maximum number of steps per revolution of stepper 
volatile bool feedTime = false; //bool to tell whether or not to activate feeding
volatile bool resetTm = false; //bool to reset set time when interrupt is generated
uint8_t foodSet = 1;
const uint8_t motorDelay = 500; //millisecond delay on accelerate and decelerate
const uint8_t maxMotorPWM = 255;
uint8_t hourSet = 7; //Set Hour of morning feeding, night feeding will be 7+12 = 19
uint8_t minSet = 0; //Set minute of feeding, can change with manual feeding
uint8_t secSet = 0; //Set second of feeding to ensure it doesn't feed a bunch duringa single minute
const int led = 8; //pin 8
bool ledState = false;
bool blinky = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  AFMS.begin();
  pinMode(button,INPUT);
  pinMode(rotarySwitch, INPUT);
  pinMode(led,OUTPUT);
  digitalWrite(led,ledState);
  attachInterrupt(0,manualFeeding,FALLING);  
}

void loop() {
  //Check Food Setting
  foodSet = foodSetting();
  //Check for time reset
  setTimeReset();
  //Check if it's time to feed!
  checkTime();
  //Feed if it is time
  feed(feedTime);
  //blink LED if real time clock needs to be checked (battery, connection, etc.)
  if(blinky){
    ledState = !ledState;
    digitalWrite(led,ledState); 
  } else {
    ledState = false;
    digitalWrite(led,ledState);
  }
  //Regulate the Loop time
  regulateLoop(dt);
}

void manualFeeding(){
  if (millis()-lastInterrupt > 20){
  feedTime = true;
  resetTm = true;
  lastInterrupt = millis();
  }
  
}
void setTimeReset(){
  if (resetTm){
    Serial.println("Interrupt Generated");
    // Get current time and update set time
    tmElements_t tm;
    if (RTC.read(tm)){
      hourSet = tm.Hour;
      if (hourSet >= 12){
        hourSet -= 12;
      }
      minSet = tm.Minute;
      secSet = tm.Second;
      Serial.print("Time Reset to: ");
      Serial.print(hourSet); Serial.print(" "); Serial.print(minSet); Serial.println();  
    } else {
    
    }
    resetTm = false;
  }
}

void checkTime (){ //This function sets the global variable feedTime
  //Check for interrupt generated feeding
  if (feedTime){
    return; //pass it on through!
  }
  tmElements_t tm;
  if (RTC.read(tm)){
    blinky = false;
    if (tm.Hour == hourSet || tm.Hour == hourSet+12){
      if (tm.Minute == minSet){
        if (tm.Second == secSet){
          feedTime = true; //will only make it to here if feedTime was initially false
        }
      }
    }
    return;
  }else{
     if (RTC.chipPresent()){
       blinky = true;
       Serial.println("Check RTC");
    } else {
       blinky = true;
       Serial.println("Check RTC");
    }
    feedTime = false;
    return;
  }
}

void feed(bool isLunchTime){
  if (isLunchTime){
    myMotor->run(FORWARD);
    //Accelerate();
    //Deccelerate();
    myMotor->setSpeed(maxMotorPWM);
    delay(foodSet*motorDelay);
    myMotor->run(RELEASE);
    feedTime = false;
  }
  
}
int foodSetting (){
  int foodSetting = 1;
  int raw = analogRead(rotarySwitch);
  foodSetting = round(raw/voltageBitDivision)+1;
  Serial.println(foodSetting);
  return(foodSetting);
}

void regulateLoop(float dt){
  static uint32_t prev_time = 0;
  uint32_t dT = uint32_t(dt*1000000);
  while((micros()-prev_time)<dT)
  {
    // do nothing
  }
  prev_time = micros();
}

void Accelerate (){
  uint8_t i;
  for (i=1; i < maxMotorPWM; i++){
    myMotor->setSpeed(i);
    delay(foodSet*motorDelay);  
  }
}

void Deccelerate () {
  uint8_t i;
  for (i = maxMotorPWM; i!=0; i--){
    myMotor->setSpeed(i);
    delay(foodSet*motorDelay);
  }
}

