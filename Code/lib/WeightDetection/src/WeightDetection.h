#ifndef WEIGHTDETECTION_H
#define WEIGHTDETECTION_H

#include <TOF.h>

typedef enum {
    FAR_LEFT = -20,
    LEFT = -10,
    CENTER = 0,
    RIGHT = 10,
    FAR_RIGHT = 20,
    UNDEFINED = -1
} weight_direction_t;

typedef struct {
    weight_direction_t direction;
    uint16_t distance;
    uint8_t certainty;
} weight_info_t;

weight_info_t weightDetection();
void weightTask();
weight_info_t checkWeight();

#endif