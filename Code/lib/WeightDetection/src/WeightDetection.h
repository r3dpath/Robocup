#ifndef WEIGHTDETECTION_H
#define WEIGHTDETECTION_H

#include <TOF.h>

#define PERCENTAGE_THRESHOLD 1.15 // Top must be more than the bottom by this percentage
#define MIN_VALID_DIFF 400  // Minimum difference to consider a weight (filter small noise)
#define MAX_DETECTION_RANGE 1500 // Maximum distance to detect a weight

typedef enum {
    FURTHER_LEFT = -20,
    FAR_LEFT = -15,
    LEFT = -10,
    CENTER_LEFT = -5,
    CENTER = 0,
    CENTER_RIGHT = 5,
    RIGHT = 10,
    FAR_RIGHT = 15,
    FURTHER_RIGHT = 20,
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