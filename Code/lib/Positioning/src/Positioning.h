#ifndef POSIITONING_H
#define POSITIONING_H

#include <Encoder.h>
#include <IMU.h>
#include <math.h>

typedef enum {
    BASE_LEFT,
    BASE_RIGHT
} base_side_t;

#define INITIAL_ANGLE 90
#define INITIAL_SIDE BASE_RIGHT


typedef struct {
    float x;
    float y;
} position_t;


void positionTick();
void printPosition();

#endif // POSITIONING_H