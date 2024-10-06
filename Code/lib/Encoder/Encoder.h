#ifndef ENCODER_H
#define ENCODER_H

#include <Wire.h>
#include <AS5600.h>
#include <IntervalTimer.h>
#include "debug.h"

#define POS_ENCODER_TICK_FREQ 15000

enum PinAssignments {
  encoderLeftA = 30,
  encoderLeftB = 31,
  
  encoderRightA = 32,
  encoderRightB = 33,
};


void initEncoder();
void printEncoderCount();
int32_t getPosEncoderCount();
float getLeftEncoderSpeed();
float getRightEncoderSpeed();
int32_t getPosEncoderDiff();
void printEncoderCount();


#endif