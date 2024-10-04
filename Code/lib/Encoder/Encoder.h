#ifndef ENCODER_H
#define ENCODER_H

#include <i2c_driver_wire.h>
#include <SparkFunSX1509.h>

enum PinAssignments {
  encoder1PinA = 33,
  encoder1PinB = 32,
  
  encoder2PinA = 31,
  encoder2PinB = 30,
};


void initEncoder();
void print_encodercount();
int32_t getEncoderCounts();
void tickEncoder();
int32_t getEncoderDiff();

#endif