#ifndef IMU_H
#define IMU_H

#include <Wire.h>
#include <BNO055_support.h>
#include <debug.h>

extern struct bno055_t myBNO;
extern struct bno055_euler myEulerData;
extern unsigned long lastTime;

void initIMU();
void UpdateIMU();
int16_t getIMUHeading();


#endif