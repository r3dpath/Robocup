#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <TOF.h>
#include <Servo.h>
#include <IMU.h>
#include <debug.h>

#define PPM_STOP 1500
#define SPEED_SLOW 300
#define SPEED_FAST 400

#define MOV_MIN_DISTANCE 250

void initMovement();
void movementController();
bool TurnToHeading(uint16_t TargetHeading);
void turn180();

void Stationary();
void Forward();
void Reverse();
void RightTurn();
void LeftTurn();
void SlowBackward();
void SlowForward();
void SlowLeft();
void SlowRight();
void ForwardLeft();
void ForwardRight();

#endif
