#ifndef POSIITONING_H
#define POSITIONING_H

#include <IMU.h>
#include <math.h>
#include "debug.h"

typedef enum {
    BASE_LEFT,
    BASE_RIGHT
} base_side_t;

typedef struct {
    float x;
    float y;
} position_t;

bool POS_OOB = false;

#define START_BASE BASE_LEFT
#define START_ANGLE 0

#define ARENA_WIDTH 2900
#define ARENA_LENGTH 4900

void positionTick();
position_t getPosition();
void setPosition(position_t);
void printPosition();

#endif // POSITIONING_H