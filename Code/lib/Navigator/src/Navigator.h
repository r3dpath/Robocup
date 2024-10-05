#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include <TOF.h>
#include <Positioning.h>
#include <Movement.h>

#define NAV_AVOID_DIST_MAX 300
#define NAV_CLOSE_ENOUGH_GOOD_ENOUGH 300
#define NAV_DEFAULT_SPEED 5

#define NAV_WEIGHT_DET_SPEED 3
#define NAV_WEIGHT_CERTAINTY_THRESHOLD 3
#define NAV_WEIGHT_ENGAGE_DIST 300

typedef enum {
    NAVIGATOR_PICK_POINT,
    NAVIGATOR_MOVING,
    NAVIGATOR_AVOIDING,
    NAVIGATOR_COLLECTING,
    NAVIGATOR_TERMINAL_GUIDANCE,
    NAVIGATOR_STUCK
} navigator_state_t;

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

void initNavigator();
void navigatorFSM();
void setTargetPosition(position_t target);
position_t getTargetPosition();
void setWeightDetected(weight_info_t weight);

#endif