#include "Encoder.h"

volatile int32_t leftEncoderPos = 0;
volatile int32_t rightEncoderPos = 0;
volatile int32_t magEncoderPos = 0;
int32_t lastLeftPos = 0;
int32_t lastRightPos = 0;
int32_t lastMagEncoderPos = 0;

boolean A_set1 = false;
boolean B_set1 = false;
boolean A_set2 = false;
boolean B_set2 = false;

AS5600 magEncoder(&Wire1);
IntervalTimer tPosEncoderTick;

// Interrupt on A changing state
void doEncoderLeft(){
  // Test transition
  A_set1 = digitalRead(encoderLeftA) == HIGH;
  // and adjust counter + if A leads B
  leftEncoderPos += (A_set1 != B_set1) ? +1 : -1;
  
  B_set1 = digitalRead(encoderLeftB) == HIGH;
  // and adjust counter + if B follows A
  leftEncoderPos += (A_set1 == B_set1) ? +1 : -1;
}


// Interrupt on A changing state
void doEncoderRight(){
  // Test transition
  A_set2 = digitalRead(encoderRightA) == HIGH;
  // and adjust counter + if A leads B
  rightEncoderPos -= (A_set2 != B_set2) ? +1 : -1;
  
  B_set2 = digitalRead(encoderRightB) == HIGH;
  // and adjust counter + if B follows A
  rightEncoderPos -= (A_set2 == B_set2) ? +1 : -1;
}

// Update the positioning encoder. Must be called 3 times per revolution, making it somewhat time critical.
void tickEncoder() {
  magEncoderPos = magEncoder.getCumulativePosition();
}

void initEncoder()
{
  pinMode(encoderLeftA, INPUT);       //Set encoder pins as inputs
  pinMode(encoderLeftB, INPUT); 
  pinMode(encoderRightA, INPUT); 
  pinMode(encoderRightB, INPUT);

  //Set up an interrupt for each wheel encoder
  attachInterrupt(digitalPinToInterrupt(encoderLeftA), doEncoderLeft, CHANGE);  
  attachInterrupt(digitalPinToInterrupt(encoderRightA), doEncoderRight, CHANGE);

  //Set up an interrupt for updating the positioning encoder.
  tPosEncoderTick.begin(tickEncoder, POS_ENCODER_TICK_FREQ);


  magEncoder.begin();
}

// Get the total positioning encoder count.
int32_t getPosEncoderCount() {
  return magEncoderPos;
}

// Returns the speed in encoder counts per second, averaged since the last time the function was called.
float getLeftEncoderSpeed() {
  static elapsedMillis timesince = 0;
  float diff = (float)(leftEncoderPos - lastLeftPos)/((float)timesince/1000.0);
  timesince = 0;
  lastLeftPos = leftEncoderPos;
  return diff;
}


// Max speed seems to be around 6000 counts per second.

// Returns the speed in encoder counts per second, averaged since the last time the function was called.
float getRightEncoderSpeed() {
  static elapsedMillis timesince = 0;
  float diff = (float)(rightEncoderPos - lastRightPos)/((float)timesince/1000.0);
  timesince = 0;
  lastRightPos = rightEncoderPos;
  return diff;
}

// Get the difference in encoder count since the last call.
int32_t getPosEncoderDiff() {
  int32_t diff = magEncoderPos - lastMagEncoderPos;
  lastMagEncoderPos = magEncoderPos;
  return diff;
}

void printEncoderCount()
{ 
    Serial.print("Index:");
    Serial.print(leftEncoderPos, DEC);
    Serial.print(":");
    Serial.print(rightEncoderPos, DEC);
    Serial.print(":");
    Serial.print(magEncoderPos, DEC);
    Serial.println();
}


