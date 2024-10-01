#ifndef ENCODER_H
#define ENCODER_H

#include <Wire.h>
#include <SparkFunSX1509.h>

enum PinAssignments {
  encoder1PinA = 2,
  encoder1PinB = 3,
  
  encoder2PinA = 4,
  encoder2PinB = 5,
};

volatile unsigned int encoderPos1 = 0;
unsigned int lastReportedPos1 = 1;
volatile unsigned int encoderPos2 = 0;
unsigned int lastReportedPos2 = 1;

boolean A_set1 = false;
boolean B_set1 = false;
boolean A_set2 = false;
boolean B_set2 = false;

#endif