#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <Servo.h>
#include <IMU.h>
#include <Encoder.h>
#include <debug.h>
#include <TOF.h>

#define PPM_STOP 1500
#define SPEED_SLOW 250
#define SPEED_FAST 380

#define MOV_MIN_DISTANCE 150

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
uint16_t getBackTOFreading();
#endif
