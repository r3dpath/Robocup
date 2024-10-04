#ifndef POSIITONING_H
#define POSITIONING_H

#include <Encoder.h>
#include <IMU.h>
#include <math.h>

typedef enum {
    LEFT,
    RIGHT
} base_side_t;

#define INITIAL_ANGLE 90
#define INITIAL_SIDE RIGHT


typedef struct {
    float x;
    float y;
} position_t;


void positionTick();
void printPosition();

#endif // POSITIONING_H