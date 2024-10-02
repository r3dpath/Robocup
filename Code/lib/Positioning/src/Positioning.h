#ifndef POSIITONING_H
#define POSITIONING_H

#include <Encoder.h>
#include <IMU.h>
#include <math.h>

typedef struct {
    float x;
    float y;
} position_t;


void positionTick();
void printPosition();

#endif // POSITIONING_H