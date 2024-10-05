#include "Navigator.h"

navigator_state_t navigator_state = NAVIGATOR_PICK_POINT;

uint8_t no_move_count = 0;
position_t last_position = {0, 0};
position_t target_position = {0, 0};
weight_info_t weight_detected = {UNDEFINED, -1, 0};

extern elapsedMillis round_time;
elapsedMillis terminalGuide_time = 0;

void pickPoint_s();
void moving_s();
void terminalGuidance_s();
uint16_t distanceToTarget();


void navigatorFSM() {
    position_t current_position = getPosition();
    switch (navigator_state) {
        case NAVIGATOR_PICK_POINT:
            pickPoint_s();
            break;
        case NAVIGATOR_MOVING:
            moving_s();
            break;
        case NAVIGATOR_AVOIDING:
            avoiding_s();
            break;
        case NAVIGATOR_COLLECTING:
            collecting_s();
            break;
        case NAVIGATOR_TERMINAL_GUIDANCE:
            terminalGuidance_s();
            break;
        case NAVIGATOR_SUCK:
            suck_s();
            break;
    }

    // Check if the robot is stuck
    if (last_position.x == current_position.x && last_position.y == current_position.y) {
        no_move_count++;
    } else {
        no_move_count = 0;
    }
    if (no_move_count > 10) {
        navigator_state = NAVIGATOR_SUCK;
    }
    last_position = current_position;
}

void pickPoint_s() {
    position_t current_position = getPosition();
    static uint16_t iter = 0;
    if (iter == 0) {
        target_position = {current_position.x, current_position.y + 2000};
    }
    // TODO: Continue filling out, grid search. Also have a before avoid point, to revert to

    iter += 1;
    navigator_state = NAVIGATOR_MOVING;
    // Could stop and do a scanning turn at each point maybe?
}

// Moves to the target position, unless an obstacle impedes path
void moving_s() {
    static elapsedMillis last_move = 0;
    uint16_t dist = distanceToTarget();
    if (dist < NAV_CLOSE_ENOUGH_GOOD_ENOUGH) {
        navigator_state = NAVIGATOR_PICK_POINT;
        last_move = 0;
        return;
    }
    turnToPosition(target_position);
    setMovementSpeed(NAV_DEFAULT_SPEED);
    obstacleDetection();
}

void avoiding_s() {

}

void collecting_s() {
    turnToWeight(weight_detected);
    setMovementSpeed(NAV_WEIGHT_DET_SPEED);
    if (weight_detected.distance < NAV_WEIGHT_ENGAGE_DIST) {
        terminalGuide_time = 0;
        navigator_state = NAVIGATOR_TERMINAL_GUIDANCE;
        terminalGuidance_s();
    }
}

void terminalGuidance_s() {
    static position_t terminalPosition = {0, 0};
    if (terminalGuide_time < 1) {
        position_t centering = {-60, 300}; // Try to allow for the offset 
        turnToPosition(centering);
    }
}

void suck_s() {

}

void setTargetPosition(position_t target) {
    target_position = target;
}

position_t getTargetPosition() {
    return target_position;
}

void setWeightDetected(weight_info_t weight) {
    weight_detected = weight;
    if (weight.certainty >= NAV_WEIGHT_CERTAINTY_THRESHOLD) {
        navigator_state = NAVIGATOR_COLLECTING;
    }
}

// Turns to an absolute position
void turnToPosition(position_t target) {
    position_t current = getPosition();
    float dx = target.x - current.x;
    float dy = target.y - current.y;
    float angle = atan2(dy, dx);
    setMovementHeading(angle);
}

// Turns to a relative position
void setRelative(position_t target) {
    target_position = {target_position.x + target.x, target_position.y + target.y};
}

// Turns to a weight
void turnToWeight(weight_info_t weight) {
    int16_t angle = getIMUHeading() + weight.direction;
    setMovementHeading(angle);
}

void obstacleDetection() {
    uint16_t l_dist = tof_l.read();
    uint16_t r_dist = tof_r.read();
    uint16_t f_dist = tof_scan_right.f_distance;

    if (l_dist < NAV_AVOID_DIST_MAX || r_dist < NAV_AVOID_DIST_MAX || f_dist < NAV_AVOID_DIST_MAX) {
        navigator_state = NAVIGATOR_AVOIDING;
    }
}

uint16_t distanceToTarget() {
    position_t current = getPosition();
    float dx = target_position.x - current.x;
    float dy = target_position.y - current.y;
    return sqrt(dx*dx + dy*dy);
}