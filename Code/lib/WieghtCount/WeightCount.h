#ifndef WEIGHTCOUNT_H
#define SWEIGHTCOUNT_H

#include <TOF.h>
#include <Wire.h>
#include <Movement.h>
extern SX1509 io;

void CheckWeightCount();
uint8_t getWeightCount();

#endif