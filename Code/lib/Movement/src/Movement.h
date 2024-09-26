#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <TOF.h>
#include <Servo.h>

#define PPM_STOP 1500
#define SPEED_SLOW 360
#define SPEED_FAST 450

void initMovement();
void movementController();

void Stationary();
void Forward();
void Reverse();
void RightTurn();
void LeftTurn();
void SlowBackward();
void SlowForward();
void SlowLeft();
void SlowRight();

#endif
