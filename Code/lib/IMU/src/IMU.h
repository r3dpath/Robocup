#ifndef IMU_H
#define IMU_H

#include <i2c_driver_wire.h>
#include <BNO055_support.h>
#include <debug.h>
#include <Movement.h>

extern struct bno055_t myBNO;
extern struct bno055_euler myEulerData;
extern unsigned long lastTime;

void initIMU();
void UpdateIMU();
uint16_t getIMUHeading();


#endif