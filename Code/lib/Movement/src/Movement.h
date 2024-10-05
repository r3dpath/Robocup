#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <Servo.h>
#include <IMU.h>
#include <Encoder.h>

#include <debug.h>

#define PPM_STOP 1500
#define SPEED_MAX 450

#define PIN_LEFT_MOTOR 28
#define PIN_RIGHT_MOTOR 29

#define MOV_MIN_DISTANCE 150

#define MOVEMENT_P 50
#define MOVEMENT_I 1
#define MOVEMENT_HEADING_MULT 5

void initMovement();
void movementController();
void setMovementHeading(int16_t heading);
void incrementMovementHeading(int16_t heading);
int16_t getMovementHeading();
void setMovementSpeed(int8_t speed);
int8_t getMovementSpeed();

#endif
