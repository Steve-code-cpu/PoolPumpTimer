#include <virtuabotixRTC.h>

#include <virtuabotixRTC.h>

// Rotary Encoder Inputs
#define CLK 2
#define DT 3
#define SW 4

int counter = 0;
int currentStateCLK;
int lastStateCLK;
// LED
int redpin = 11; //select the pin for the red LED
int bluepin = 10; // select the pin for the  blue LED
int greenpin = 9; // select the pin for the green LED
int val = 255;
int rotaryPosition = 1;

int firstStartTimeHour = 19;
int firstStartTimeMinute = 54;
int firstEndTimeHour = 19;
int firstEndTimeMinute = 55;

bool isPumpOn = false;
bool isLightOn = false;

//Clock
virtuabotixRTC myRTC(6, 7, 8);

String currentDir = "";
unsigned long lastButtonPress = 0;

void setup() {

  //Clock
  //TODO: Set this to the exact time programatically
  //   myRTC.setDS1302Time(0, 52, 19, 1, 17, 10, 2021);

  // Set encoder pins as inputs
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);

  //LED
  pinMode(redpin, OUTPUT);
  pinMode(bluepin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  changeLedColor(rotaryPosition);

  // Setup Serial Monitor
  Serial.begin(9600);

  // Read the initial state of CLK
  lastStateCLK = digitalRead(CLK);
}

void loop() {



  //Clock
  myRTC.updateTime();

  Serial.print("Current Date / Time: ");
  Serial.print(myRTC.dayofmonth); //You can switch between day and month if you're using American system
  Serial.print("/");
  Serial.print(myRTC.month);
  Serial.print("/");
  Serial.print(myRTC.year);
  Serial.print(" ");
  Serial.print(myRTC.hours);
  Serial.print(":");
  Serial.print(myRTC.minutes);
  Serial.print(":");
  Serial.println(myRTC.seconds);

  switch (rotaryPosition) {
    case 1:
      handleRotaryPosition1();
      break;
    case 2:
      handleRotaryPosition2();
      break;
    case 3:
      handleRotaryPosition3();
      break;
    case 4:
      handleRotaryPosition4();
      break;
    default:
      break;
  }

  Serial.print(isPumpOn);
  Serial.print(isLightOn);

  // Read the current state of CLK
  currentStateCLK = digitalRead(CLK);

  // If last and current state of CLK are different, then pulse occurred
  // React to only 1 state change to avoid double count
  if (currentStateCLK != lastStateCLK  && currentStateCLK == 1) {

    // If the DT state is different than the CLK state then
    // the encoder is rotating CCW so decrement
    if (digitalRead(DT) != currentStateCLK) {

      if (rotaryPosition == 1) {
        rotaryPosition = 4;
      } else {
        rotaryPosition --;
      }

      counter --;
      currentDir = "CCW";
    } else {

      if (rotaryPosition == 4) {
        rotaryPosition = 1;
      } else {
        rotaryPosition ++;
      }

      // Encoder is rotating CW so increment
      counter ++;
      currentDir = "CW";
    }

    changeLedColor(rotaryPosition);

    Serial.print("Direction: ");
    Serial.print(currentDir);
    Serial.print(" | Counter: ");
    Serial.println(counter);
  }

  // Remember last CLK state
  lastStateCLK = currentStateCLK;

  // Read the button state
  int btnState = digitalRead(SW);

  //If we detect LOW signal, button is pressed
  if (btnState == LOW) {
    //if 50ms have passed since last LOW pulse, it means that the
    //button has been pressed, released and pressed again
    if (millis() - lastButtonPress > 50) {
      Serial.println("Button pressed!");
    }

    // Remember last button press event
    lastButtonPress = millis();
  }

  // Put in a slight delay to help debounce the reading
  delay(1);
}

//Functions

int changeLedColor(int color) {
  switch (color) {
    case 1:
      analogWrite(11, 0);
      analogWrite(10, 0);
      analogWrite(9, 0);
      break;
    case 2:
      analogWrite(11, 255);
      analogWrite(10, 0);
      analogWrite(9, 0);
      break;
    case 3:
      analogWrite(11, 0);
      analogWrite(10, 0);
      analogWrite(9, 255);
      break;
    case 4:
      analogWrite(11, 0);
      analogWrite(10, 255);
      analogWrite(9, 0);
      break;
    default:
      analogWrite(11, 0);
      analogWrite(10, 0);
      analogWrite(9, 0);
      break;
  }
}

int checkIfPumpShouldRun(int startTimeHour, int startTimeMinute, int endTimeHour, int endTimeMinute) {

  int currentTime = (myRTC.hours * 60) + myRTC.minutes;
  int startTime = (startTimeHour * 60) + startTimeMinute;
  int endTime = (endTimeHour * 60) + endTimeMinute;

  boolean shouldPumpRun;

  if (endTime > startTime) {
    shouldPumpRun = (currentTime >= startTime && currentTime < endTime);
  } else {
    shouldPumpRun = (currentTime >= startTime || currentTime < endTime);
  }

  Serial.print(shouldPumpRun);

  return shouldPumpRun;

}

int getCurrentTime() {
  //TODO
}

void handleRotaryPosition1() {
  isPumpOn = false;
  isLightOn = false;
}

void handleRotaryPosition2() {
  isPumpOn = false;
  isLightOn = true;
}

void handleRotaryPosition3() {
  isPumpOn = true;
  isLightOn = true;
}

void handleRotaryPosition4() {
  boolean shouldPumpRun = checkIfPumpShouldRun(firstStartTimeHour, firstStartTimeMinute, firstEndTimeHour, firstEndTimeMinute);
  if (shouldPumpRun != isPumpOn) {
    isPumpOn = shouldPumpRun;
    isLightOn = shouldPumpRun;
  }
}
