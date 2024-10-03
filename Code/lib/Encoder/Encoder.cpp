#include "Encoder.h"
#include "debug.h"

volatile int32_t encoderPos[3];
int32_t lastReportedPos1 = 0;
int32_t lastReportedPos2 = 0;
int32_t lastReportedPos3 = 0;

boolean A_set1 = false;
boolean B_set1 = false;
boolean A_set2 = false;
boolean B_set2 = false;
boolean A_set3 = false;
boolean B_set3 = false;

// Interrupt on A changing state
void doEncoder1A(){
  // Test transition
  A_set1 = digitalRead(encoder1PinA) == HIGH;
  // and adjust counter + if A leads B
  encoderPos[0] += (A_set1 != B_set1) ? +1 : -1;
  
  B_set1 = digitalRead(encoder1PinB) == HIGH;
  // and adjust counter + if B follows A
  encoderPos[0] += (A_set1 == B_set1) ? +1 : -1;
}


// Interrupt on A changing state
void doEncoder2A(){
  // Test transition
  A_set2 = digitalRead(encoder2PinA) == HIGH;
  // and adjust counter + if A leads B
  encoderPos[1] -= (A_set2 != B_set2) ? +1 : -1;
  
  B_set2 = digitalRead(encoder2PinB) == HIGH;
  // and adjust counter + if B follows A
  encoderPos[1] -= (A_set2 == B_set2) ? +1 : -1;
}

// Interrupt on A changing state
void doEncoder3A(){
  // Test transition
  A_set2 = digitalRead(encoder3PinA) == HIGH;
  // and adjust counter + if A leads B
  encoderPos[2] += (A_set2 != B_set2) ? +1 : -1;
  
  B_set2 = digitalRead(encoder3PinB) == HIGH;
  // and adjust counter + if B follows A
  encoderPos[2] += (A_set2 == B_set2) ? +1 : -1;
}

void initEncoder()
{
  pinMode(encoder1PinA, INPUT);       //Set encoder pins as inputs
  pinMode(encoder1PinB, INPUT); 
  pinMode(encoder2PinA, INPUT); 
  pinMode(encoder2PinB, INPUT);
  pinMode(encoder3PinA, INPUT); 
  pinMode(encoder3PinB, INPUT);  

  attachInterrupt(digitalPinToInterrupt(encoder1PinA), doEncoder1A, CHANGE);  //Set up an interrupt for each encoder
  attachInterrupt(digitalPinToInterrupt(encoder2PinA), doEncoder2A, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoder3PinA), doEncoder3A, CHANGE);
}


void print_encodercount()
{ 
  //If there has been a change in value of either encoder then print the 
  //  encoder values to the serial port
  Serial.println(encoderPos[2]);
  if ((lastReportedPos1 != encoderPos[0])||(lastReportedPos2 != encoderPos[1])||(lastReportedPos3 != encoderPos[2])) 
  {
    Serial.print("Index:");
    Serial.print(encoderPos[0], DEC);
    Serial.print(":");
    Serial.print(encoderPos[1], DEC);
    Serial.print(":");
    Serial.print(encoderPos[2], DEC);
    Serial.println();
    lastReportedPos1 = encoderPos[0];
    lastReportedPos2 = encoderPos[1];
  }
}

int32_t* getEncoderCounts()
{
  return encoderPos;
}

// Average together both encoders to get the total change in encoder count between iterations
int32_t getEncoderDiff()
{
  // int32_t diff = (encoderPos[0] - lastReportedPos1) + (encoderPos[1] - lastReportedPos2);
  // diff /= 2;
  // lastReportedPos1 = encoderPos[0];
  // lastReportedPos2 = encoderPos[1];
  int32_t diff = encoderPos[2] - lastReportedPos3;
  lastReportedPos3 = encoderPos[2];
  return diff;
}


