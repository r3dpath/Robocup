#include "Encoder.h"
#include "debug.h"

volatile int32_t encoderPos[2];
int32_t lastReportedPos1 = 1;
int32_t lastReportedPos2 = 1;

boolean A_set1 = false;
boolean B_set1 = false;
boolean A_set2 = false;
boolean B_set2 = false;

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

void initEncoder()
{
  pinMode(encoder1PinA, INPUT);       //Set encoder pins as inputs
  pinMode(encoder1PinB, INPUT); 
  pinMode(encoder2PinA, INPUT); 
  pinMode(encoder2PinB, INPUT); 

  attachInterrupt(digitalPinToInterrupt(33), doEncoder1A, CHANGE);  //Set up an interrupt for each encoder
  attachInterrupt(digitalPinToInterrupt(31), doEncoder2A, CHANGE);
}


void print_encodercount()
{ 
  //If there has been a change in value of either encoder then print the 
  //  encoder values to the serial port
  if ((lastReportedPos1 != encoderPos[0])||(lastReportedPos2 != encoderPos[1])) 
  {
    Serial.print("Index:");
    Serial.print(encoderPos[0], DEC);
    Serial.print(":");
    Serial.print(encoderPos[1], DEC);
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
  int32_t diff = (encoderPos[0] - lastReportedPos1) + (encoderPos[1] - lastReportedPos2);
  diff /= 2;
  lastReportedPos1 = encoderPos[0];
  lastReportedPos2 = encoderPos[1];
  return diff;
}


