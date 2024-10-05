#ifndef ENCODER_H
#define ENCODER_H

#include <Wire.h>
#include <AS5600.h>
#include <IntervalTimer.h>
#include "debug.h"

#define POS_ENCODER_TICK_FREQ 15000

enum PinAssignments {
  encoderLeftA = 33,
  encoderLeftB = 32,
  
  encoderRightA = 31,
  encoderRightB = 30,
};


void initEncoder();
void printEncoderCount();
int32_t getPosEncoderCount();
float getLeftEncoderSpeed();
float getRightEncoderSpeed();
int32_t getPosEncoderDiff();
void printEncoderCount();


#endif