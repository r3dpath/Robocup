#ifndef WEIGHTDETECTION_H
#define WEIGHTDETECTION_H

#include <TOF.h>

typedef enum {
    FAR_LEFT = 0,
    LEFT = 1,
    CENTER = 2,
    RIGHT = 3,
    FAR_RIGHT = 4,
    UNDEFINED = -1
} weight_direction_t;

typedef struct {
    weight_direction_t direction;
    uint16_t distance;
    uint8_t certainty;
} weight_info_t;

weight_info_t weightDetection();

#endif