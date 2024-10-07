#ifndef POSIITONING_H
#define POSITIONING_H

#include <IMU.h>
#include <math.h>
#include "debug.h"
#include "Encoder.h"
#include <Movement.h>

typedef enum {
    BASE_LEFT,
    BASE_RIGHT
} base_side_t;

typedef struct {
    float x;
    float y;
} position_t;

extern bool POS_OOB;

#define START_BASE BASE_LEFT
// START ANGLE IN MOVEMENT.H

#define ARENA_WIDTH 2900
#define ARENA_LENGTH 4900
#define ARENA_BUFFER 150

void positionTick();
position_t getPosition();
void setPosition(position_t);
void printPosition();

#endif // POSITIONING_H