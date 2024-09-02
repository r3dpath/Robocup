#ifndef IMU_H
#define IMU_H

#include <Wire.h>
#include <VL53L0X.h>
#include <VL53L1X.h>
#include <SparkFunSX1509.h>
#include <Servo.h>
#include <QuadEncoder.h>
#include "BNO055_support.h"

extern struct bno055_t myBNO;
extern struct bno055_euler myEulerData;
extern unsigned long lastTime;

void UpdateIMU();
void IMU_setup();

#endif