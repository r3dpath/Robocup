#include "Encoder.h"
#include "debug.h"


enum PinAssignments {
  encoder1PinA = 33,
  encoder1PinB = 32,
  
  encoder2PinA = 31,
  encoder2PinB = 30,
};

volatile unsigned int encoderPos1 = 0;
unsigned int lastReportedPos1 = 1;
volatile unsigned int encoderPos2 = 0;
unsigned int lastReportedPos2 = 1;

boolean A_set1 = false;
boolean B_set1 = false;
boolean A_set2 = false;
boolean B_set2 = false;

// Interrupt on A changing state
void doEncoder1A(){
  // Test transition
  A_set1 = digitalRead(encoder1PinA) == HIGH;
  // and adjust counter + if A leads B
  encoderPos1 += (A_set1 != B_set1) ? +1 : -1;
  
  B_set1 = digitalRead(encoder1PinB) == HIGH;
  // and adjust counter + if B follows A
  encoderPos1 += (A_set1 == B_set1) ? +1 : -1;
}


// Interrupt on A changing state
void doEncoder2A(){
  // Test transition
  A_set2 = digitalRead(encoder2PinA) == HIGH;
  // and adjust counter + if A leads B
  encoderPos2 -= (A_set2 != B_set2) ? +1 : -1;
  
  B_set2 = digitalRead(encoder2PinB) == HIGH;
  // and adjust counter + if B follows A
  encoderPos2 -= (A_set2 == B_set2) ? +1 : -1;
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
  if ((lastReportedPos1 != encoderPos1)||(lastReportedPos2 != encoderPos2)) 
  {
    Serial.print("Index:");
    Serial.print(encoderPos1, DEC);
    Serial.print(":");
    Serial.print(encoderPos2, DEC);
    Serial.println();
    lastReportedPos1 = encoderPos1;
    lastReportedPos2 = encoderPos2;
  }
}



