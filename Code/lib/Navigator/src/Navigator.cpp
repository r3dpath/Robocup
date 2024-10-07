#include "Navigator.h"

extern TOF tof_l;
extern TOF tof_r;
extern TOF2 tof_scan_left;
extern TOF2 tof_scan_right;

navigator_state_t navigator_state = NAVIGATOR_PICK_POINT;

map_point_t targets[LEN_MAP_POINTS];
position_t current_target = {-1, -1};
uint8_t target_pointer = 0;

uint8_t no_move_count = 0;
weight_info_t weight_detected = {UNDEFINED, -1, 0};

extern elapsedMillis round_time;
extern position_t homePosition;
elapsedMillis terminalGuide_time = 0;
elapsedMillis stuck_time = 0;
elapsedMillis avoid_time = 0;

position_t last_position = {0, 0};

void pickPoint_s();
void moving_s();
void terminalGuidance_s();
void stuck_s();
void avoiding_s();
void collecting_s();
void addPoint(map_point_t target);
void turnToPosition(position_t target);
void turnToWeight(weight_info_t weight);
void obstacleDetection();
void popTarget();
map_point_t getNewTarget();
position_t relToAbsPos(position_t target);
int16_t angleToTarget();

uint16_t distanceToTarget();

void initNavigator() {
    position_t current_position = getPosition();
    last_position = getPosition();
    // Can add custom targets here. REVERSE ORDER!
    /*
    addTarget({1000, 1000}, false);
    addTarget({2000, 2000}, false);
    */
    addPoint((map_point_t){2128.20, 334.40, 0});
    addPoint((map_point_t){228.31, 2810.66, 0});
    addPoint((map_point_t){2117.33, 4427.41, 0});
    addPoint((map_point_t){312.57, 4435.70, 0});
    addPoint((map_point_t){288.11, 2293.85, 0});
    addPoint((map_point_t){2185.28, 2296.62, 0});
    addPoint((map_point_t){2160.82, 1033.62, 0});
    addPoint((map_point_t){309.85, 1069.54, 0});
        
    
}

void navigatorFSM() {
    static elapsedMillis round_time = 0;
    position_t current_position = getPosition();
    switch (navigator_state) {
        case NAVIGATOR_PICK_POINT:
            pickPoint_s();
            break;
        case NAVIGATOR_MOVING:
            moving_s();
            break;
        case NAVIGATOR_AVOIDING:
            //avoiding_s();
            navigator_state = NAVIGATOR_MOVING;
            break;
        case NAVIGATOR_COLLECTING:
            collecting_s();
            break;
        case NAVIGATOR_TERMINAL_GUIDANCE:
            terminalGuidance_s();
            break;
        case NAVIGATOR_STUCK:
            stuck_s();
            break;
    }

    // Check if the robot is stuck
    if (navigator_state != NAVIGATOR_STUCK) {
        if (last_position.x == current_position.x && last_position.y == current_position.y) {
            no_move_count++;
        } else {
            no_move_count = 0;
        }
        if (no_move_count > 10) {
            stuck_time = 0;
            navigator_state = NAVIGATOR_STUCK;
        }
    } else {
        if (last_position.x == current_position.x && last_position.y == current_position.y) {
            navigator_state = NAVIGATOR_MOVING;
        }
    }
    
    last_position = current_position;

    #ifdef DEBUG
    Serial.print("NAV: ");
    switch (navigator_state) {
        case NAVIGATOR_PICK_POINT:
            Serial.println("PICK_POINT");
            break;
        case NAVIGATOR_MOVING:
            Serial.println("MOVING");
            break;
        case NAVIGATOR_AVOIDING:
            Serial.println("AVOIDING");
            break;
        case NAVIGATOR_COLLECTING:
            Serial.println("COLLECTING");
            break;
        case NAVIGATOR_TERMINAL_GUIDANCE:
            Serial.println("TERMINAL_GUIDANCE");
            break;
        case NAVIGATOR_STUCK:
            Serial.println("STUCK");
            break;
    }
    #endif

    #ifdef DEBUG_NAV
    Serial.print("T:");
    Serial.print(current_target.x);
    Serial.print(":");
    Serial.println(current_target.y);
    #endif
}

void pickPoint_s() {
    position_t current_position = getPosition();
    static uint16_t iter = 0;

    #ifdef DEBUG_NAV
    Serial.print("Pick point: ");
    Serial.print(iter);
    Serial.print(":");
    Serial.println(target_pointer);
    #endif

    if (target_pointer != 0) {
        if (current_target.x != targets[target_pointer-1].x && current_target.y != targets[target_pointer-1].y) {
            Serial.print("Setting new target: ");
            Serial.print(target_pointer);
            map_point_t map = getNewTarget();  
            current_target = {map.x, map.y};
            Serial.print(" : ");
            Serial.print(current_target.x);
            Serial.print(", ");
            Serial.println(current_target.y);
            iter += 1;
            navigator_state = NAVIGATOR_MOVING;
        } else {
            // If the robot has reached the target, pop it off the stack
            popTarget();
            if (target_pointer != 0) {
                Serial.print("Setting new target: ");
                Serial.print(target_pointer);   
                map_point_t map = getNewTarget();  
                current_target = {map.x, map.y};
                Serial.print(" : ");
                Serial.print(current_target.x);
                Serial.print(", ");
                Serial.println(current_target.y);
                iter += 1;
                navigator_state = NAVIGATOR_MOVING;
            }
        }
    } else {
            setMovementSpeed(0);
            exit(0);
    }
    // Could stop and do a scanning turn at each point maybe?
}

// Moves to the target position, unless an obstacle impedes path
void moving_s() {
    static elapsedMillis last_move = 0;
    uint16_t dist = distanceToTarget();
    Serial.print("Distance: ");
    Serial.println(dist);
    if (dist < NAV_CLOSE_ENOUGH_GOOD_ENOUGH) {
        navigator_state = NAVIGATOR_PICK_POINT;
        last_move = 0;
        return;
    }
    turnToPosition(current_target);
    if (dist < 3 * NAV_CLOSE_ENOUGH_GOOD_ENOUGH) {
        setMovementSpeed(NAV_DEFAULT_SPEED-2);
    } else {
        setMovementSpeed(NAV_DEFAULT_SPEED);
    }

    // weight_info_t check = checkWeight();
    // if (check.certainty >= NAV_WEIGHT_CERTAINTY_THRESHOLD) {
    //     setWeightDetected(check);
    // }

    obstacleDetection();
}

// Avoids an obstacle
void avoiding_s() {
    avoid_time = 0;
    position_t current_position = getPosition();
    int16_t angle = getBodyHeading();
    uint16_t l_dist = tof_l.read();
    uint16_t r_dist = tof_r.read();
    uint16_t f_dist = tof_scan_right.f_distance;

    // This is gonna be a bit cooked. Basic philosophy is a modified wall follow.
    // If the robot has started in the left base it should turn right and move a bit before trying to get back to the original point. Opposite if it starts in the right base.
    // As this state will be constantly entered from the moving state while avoiding obstacles, it should only avoid while the target is roughly infront of the robot.
    #if START_BASE == BASE_LEFT
        if (r_dist > NAV_AVOID_DIST_MAX) { // If clear to the right
            if (f_dist > NAV_AVOID_DIST_MAX * 2) { // If clear in front
                current_target = relToAbsPos({300+NAV_CLOSE_ENOUGH_GOOD_ENOUGH, 300}); // Slight right turn
            } else {
                current_target = relToAbsPos({300+NAV_CLOSE_ENOUGH_GOOD_ENOUGH, 0}); // Hard right turn
            }
        } else { // Must be blocked to the right so same thing but with should go back a bit too
            if (f_dist > NAV_AVOID_DIST_MAX * 2) { // If clear in front
                current_target = relToAbsPos({-300-NAV_CLOSE_ENOUGH_GOOD_ENOUGH, 300}); // Slight left turn
            } else {
                current_target = relToAbsPos({-300-NAV_CLOSE_ENOUGH_GOOD_ENOUGH, -150-NAV_CLOSE_ENOUGH_GOOD_ENOUGH}); // Hard left with some reverse
            }
        }
    #else // Same but flipped
        if (l_dist > NAV_AVOID_DIST_MAX) { // If clear to the left
            if (f_dist > NAV_AVOID_DIST_MAX) { // If clear in front
                current_target = relToAbsPos({-100-NAV_CLOSE_ENOUGH_GOOD_ENOUGH, 300}); // Slight left turn
            } else {
                current_target = relToAbsPos({-300-NAV_CLOSE_ENOUGH_GOOD_ENOUGH, 0}); // Hard left turn
            }
        } else { // Must be blocked to the left so same thing but with should go back a bit too
            if (f_dist > NAV_AVOID_DIST_MAX) { // If clear in front
                current_target = relToAbsPos({100+NAV_CLOSE_ENOUGH_GOOD_ENOUGH, 300}); // Slight right turn
            } else {
                current_target = relToAbsPos({300+NAV_CLOSE_ENOUGH_GOOD_ENOUGH, -300-NAV_CLOSE_ENOUGH_GOOD_ENOUGH}); // Hard right with some reverse
            }
        }
    #endif
    turnToPosition(current_target);
    navigator_state = NAVIGATOR_MOVING;
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
    collectionOn();
    if (terminalGuide_time < 1) {
        position_t centering = {-60, 300}; // Try to allow for the offset 
        turnToPosition(centering);
    }
    if (terminalGuide_time > 2000) {
        collectionOff();
        navigator_state = NAVIGATOR_MOVING;
    }
}

void stuck_s() {
    if (tof_scan_left.bottom[4] < NAV_AVOID_DIST_MAX || tof_scan_right.bottom[0] < NAV_AVOID_DIST_MAX || tof_l.read() < NAV_AVOID_DIST_MAX || tof_r.read() < NAV_AVOID_DIST_MAX) {
        // Fang it backwards
        setMovementSpeed(-10);
    } else {
        // Fang it forwards
        setMovementSpeed(10);
    }
    if (stuck_time > 5000) {
        navigator_state = NAVIGATOR_PICK_POINT;
    }
}

void addPoint(map_point_t target) {
    #ifdef DEBUG_NAV
    Serial.print("Adding target: ");
    Serial.print(target.x);
    Serial.print(", ");
    Serial.println(target.y);
    #endif
    if (target_pointer >= LEN_MAP_POINTS) {
        return;
    }
    targets[target_pointer] = target;
    target_pointer++;
}

void popTarget() {
    if (target_pointer > 0) {
        target_pointer--;
    }
}

map_point_t getNewTarget() {
    return targets[target_pointer-1];
}

//If uncertainty is high, navigator state switches to  collecting
void setWeightDetected(weight_info_t weight) {
    if (weight.certainty >= NAV_WEIGHT_CERTAINTY_THRESHOLD) {
        weight_detected = weight;
        navigator_state = NAVIGATOR_COLLECTING;
    }
}

// Turns to an absolute position
void turnToPosition(position_t target) {
    position_t current = getPosition();
    float dx = target.x - current.x;
    float dy = target.y - current.y;
    float angle = atan2(dx, dy) * 180 / PI;
    setMovementHeading(angle);
}

// Turns to a relative position
position_t relToAbsPos(position_t target) {
    position_t currentPos = getPosition();
    int16_t angle = getBodyHeading();
    float dx = target.x * cos(angle) - target.y * sin(angle);
    float dy = target.x * sin(angle) + target.y * cos(angle);

    return {currentPos.x + dx, currentPos.y + dy};
}

// Turns to a weight
void turnToWeight(weight_info_t weight) {
    int16_t angle = getBodyHeading() + weight.direction;
    setMovementHeading(angle);
}

void obstacleDetection() {
    if (avoid_time < 1000) {
        return;
    }
    uint16_t l_dist = tof_l.read();
    uint16_t r_dist = tof_r.read();
    uint16_t f_dist = tof_scan_left.top[4];

    #ifdef DEBUG_TOF
    Serial.print("S:");
    Serial.print(tof_scan_left.top[0]); Serial.print(":"); Serial.print(tof_scan_left.top[1]); Serial.print(":"); Serial.print(tof_scan_left.top[2]); Serial.print(":"); Serial.print(tof_scan_left.top[3]); Serial.print(":"); Serial.print(tof_scan_left.top[4]); Serial.print(":");
    Serial.print(tof_scan_left.bottom[0]); Serial.print(":"); Serial.print(tof_scan_left.bottom[1]); Serial.print(":"); Serial.print(tof_scan_left.bottom[2]); Serial.print(":"); Serial.print(tof_scan_left.bottom[3]); Serial.print(":"); Serial.print(tof_scan_left.bottom[4]); Serial.print(":");
    Serial.print(tof_scan_right.top[0]); Serial.print(":"); Serial.print(tof_scan_right.top[1]); Serial.print(":"); Serial.print(tof_scan_right.top[2]); Serial.print(":"); Serial.print(tof_scan_right.top[3]); Serial.print(":"); Serial.print(tof_scan_right.top[4]); Serial.print(":");
    Serial.print(tof_scan_right.bottom[0]); Serial.print(":"); Serial.print(tof_scan_right.bottom[1]); Serial.print(":"); Serial.print(tof_scan_right.bottom[2]); Serial.print(":"); Serial.print(tof_scan_right.bottom[3]); Serial.print(":"); Serial.print(tof_scan_right.bottom[4]); Serial.print(":");
    Serial.print(l_dist); Serial.print(":"); Serial.println(r_dist);
    #endif 

    if (l_dist < NAV_AVOID_DIST_MAX || r_dist < NAV_AVOID_DIST_MAX || f_dist < NAV_AVOID_DIST_MAX) {
        navigator_state = NAVIGATOR_AVOIDING;
    }
}

uint16_t distanceToTarget() {
    position_t current = getPosition();
    float dx = current_target.x - current.x;
    float dy = current_target.y - current.y;
    return sqrt(dx*dx + dy*dy);
}

int16_t angleToTarget() {
    position_t current = getPosition();
    float dx = current_target.x - current.x;
    float dy = current_target.y - current.y;
    return atan2(dx, dy) * 180 / PI;
}