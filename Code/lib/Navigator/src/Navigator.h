#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include <TOF.h>
#include <Positioning.h>
#include <Movement.h>
#include <WeightDetection.h>
#include <Collection.h>
#include <WeightCount.h>

#define NAV_AVOID_DIST_MAX 180
#define NAV_CLOSE_ENOUGH_GOOD_ENOUGH 100
#define NAV_DEFAULT_SPEED 8

#define NAV_WEIGHT_DET_SPEED 6
#define NAV_WEIGHT_CERTAINTY_THRESHOLD 1
#define NAV_WEIGHT_ENGAGE_DIST 300

#define LEN_MAP_POINTS 50


typedef enum {
    NAVIGATOR_PICK_POINT,
    NAVIGATOR_MOVING,
    NAVIGATOR_AVOIDING,
    NAVIGATOR_COLLECTING,
    NAVIGATOR_TERMINAL_GUIDANCE,
    NAVIGATOR_STUCK
} navigator_state_t;

typedef struct {
    float x;
    float y;
    bool isWeight;
} map_point_t;

void initNavigator();
void navigatorFSM();
void setTargetPosition(position_t target);
map_point_t getTargetPosition();
void setWeight(weight_info_t weight);

#endif