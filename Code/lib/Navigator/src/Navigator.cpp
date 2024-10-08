#include "Navigator.h"

extern TOF tof_l;
extern TOF tof_r;
extern TOF2 tof_scan_left;
extern TOF2 tof_scan_right;

navigator_state_t navigator_state = NAVIGATOR_PICK_POINT;

map_point_t targets[LEN_MAP_POINTS];
position_t current_target = {-1, -1};
uint8_t target_pointer = 0;

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
uint16_t distanceBetweenPoints(position_t* positions);
void setTarget(position_t candidate);
double cosd (double angle);
double sind (double angle);


uint16_t distanceToTarget();

void initNavigator() {
    position_t current_position = getPosition();
    last_position = getPosition();
    // Can add custom targets here. REVERSE ORDER!
    /*
    addTarget({1000, 1000}, false);
    addTarget({2000, 2000}, false);
    */
    addPoint((map_point_t){300, 300, 0});
    addPoint((map_point_t){1300, 300, 0});
    addPoint((map_point_t){1300, 1300, 0});
    addPoint((map_point_t){300, 1300, 0});
    addPoint((map_point_t){300, 300, 0});
    addPoint((map_point_t){1300, 300, 0});
    addPoint((map_point_t){1300, 1300, 0});
    addPoint((map_point_t){300, 1300, 0});
        
    
}

void navigatorFSM() {
    static uint8_t no_move_count = 0;
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
            setMovementSpeed(NAV_WEIGHT_DET_SPEED);
            collecting_s();
            break;
        case NAVIGATOR_TERMINAL_GUIDANCE:
            terminalGuidance_s();
            break;
        case NAVIGATOR_STUCK:
            stuck_s();
            break;
    }

    // STUCK CODE

    position_t things[2];
    things[0] = current_position;
    things[1] = last_position;
    uint16_t dist = distanceBetweenPoints(things);
    #ifdef DEBUG_NAV
    Serial.print("Dist since last: ");
    Serial.println(dist);
    #endif
    if (dist < 30) {
        no_move_count += 1;
    } else {
        no_move_count = 0;
    }
    if (no_move_count > 10) {
        stuck_time = 0;
        navigator_state = NAVIGATOR_STUCK;
    } else if (navigator_state == NAVIGATOR_STUCK) {
        navigator_state == NAVIGATOR_MOVING;
    }
    
    last_position = current_position;

    // STUCK CODE END

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

    weight_info_t check = checkWeight();
    setWeight(check);
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
            setTarget({map.x, map.y});
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
                setTarget({map.x, map.y});
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
    if (dist < NAV_CLOSE_ENOUGH_GOOD_ENOUGH) {
        navigator_state = NAVIGATOR_PICK_POINT;
        last_move = 0;
        return;
    }
    turnToPosition(current_target);
    if (dist < 3 * NAV_CLOSE_ENOUGH_GOOD_ENOUGH) {
        if (target_pointer > 0) {
            if (targets[target_pointer-1].isWeight) {
                navigator_state = NAVIGATOR_TERMINAL_GUIDANCE;
                terminalGuide_time = 0;
                terminalGuidance_s();
                return;
            }
        }
        setMovementSpeed(NAV_DEFAULT_SPEED-2);
    } else {
        setMovementSpeed(NAV_DEFAULT_SPEED);
    }

    obstacleDetection();
}

// Avoids an obstacle
void avoiding_s() {
    avoid_time = 0;
    position_t current_position = getPosition();
    int16_t angle = getBodyHeading();
    uint16_t l_dist = tof_l.read();
    uint16_t r_dist = tof_r.read();
    uint16_t f_dist = tof_scan_left.top[4];

    bool l = l_dist < 2*NAV_AVOID_DIST_MAX;
    bool f = f_dist < 2*NAV_AVOID_DIST_MAX;
    bool r = r_dist < 2*NAV_AVOID_DIST_MAX;


    // This is gonna be a bit cooked. Basic philosophy is a modified wall follow.
    // If the robot has started in the left base it should turn right and move a bit before trying to get back to the original point. Opposite if it starts in the right base.
    // As this state will be constantly entered from the moving state while avoiding obstacles, it should only avoid while the target is roughly infront of the robot.
    #if START_BASE == BASE_LEFT
        if (l) {
            if (f) {
                incrementMovementHeading(10);
            }
        }
    #else // Same but flipped
        if (l_dist > NAV_AVOID_DIST_MAX) { // If clear to the left
            if (f_dist > NAV_AVOID_DIST_MAX) { // If clear in front
                setTarget(relToAbsPos({-100-NAV_CLOSE_ENOUGH_GOOD_ENOUGH, 300}); // Slight left turn
            } else {
                setTarget(relToAbsPos({-300-NAV_CLOSE_ENOUGH_GOOD_ENOUGH, 0}); // Hard left turn
            }
        } else { // Must be blocked to the left so same thing but with should go back a bit too
            if (f_dist > NAV_AVOID_DIST_MAX) { // If clear in front
                setTarget(relToAbsPos({100+NAV_CLOSE_ENOUGH_GOOD_ENOUGH, 300}); // Slight right turn
            } else {
                setTarget(relToAbsPos({300+NAV_CLOSE_ENOUGH_GOOD_ENOUGH, -300-NAV_CLOSE_ENOUGH_GOOD_ENOUGH}); // Hard right with some reverse
            }
        }
    #endif
    turnToPosition(current_target);
    navigator_state = NAVIGATOR_MOVING;
}

void collecting_s() {
    turnToPosition(current_target);
    if (distanceToTarget() < NAV_WEIGHT_ENGAGE_DIST) {
        terminalGuide_time = 0;
        navigator_state = NAVIGATOR_TERMINAL_GUIDANCE;
        terminalGuidance_s();
    }
}

void terminalGuidance_s() {
    collectionOn();
    Serial.println("TERMINAL_NAV");
    // if (terminalGuide_time < 1) {
    //     position_t centering = {-60, 300}; // Try to allow for the offset 
    //     turnToPosition(relToAbsPos(centering));
    // }
    if (terminalGuide_time > 6000) {
        collectionOff();
        navigator_state = NAVIGATOR_PICK_POINT;
    }
}

//If uncertainty is high, navigator state switches to  collecting
void setWeight(weight_info_t weight) {
    weight_detected = weight;

    switch (navigator_state) {
        case NAVIGATOR_MOVING:              // If moving or picking a new point update the current target
        case NAVIGATOR_PICK_POINT:
        {
            if (weight.certainty == 0) {
                return;
            }
            #ifdef DEBUG_NAV
            Serial.print("Detected weight at angle: ");
            Serial.print(weight.direction);
            Serial.print(" and distance: ");
            Serial.println(weight.distance);
            #endif
            position_t relative = {weight.distance*sind((int8_t)weight.direction), weight.distance*cosd((int8_t)weight.direction)};
            setTarget(relToAbsPos(relative));
            #ifdef DEBUG_NAV
            Serial.print("Current body heading: ");
            Serial.println(getBodyHeading());
            Serial.print("Adding weight at position: ");
            Serial.print(current_target.x);
            Serial.print(":");
            Serial.println(current_target.y);
            #endif
            navigator_state = NAVIGATOR_COLLECTING;
            break;
        }
        case NAVIGATOR_COLLECTING:
            if (weight.certainty == 0) {
                collectionOff();
                navigator_state = NAVIGATOR_PICK_POINT;
            }
            break;
        default:
            break;
            
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
    float dx = target.x * cosd(angle) + target.y * sind(angle);
    float dy = -target.x * sind(angle) + target.y * cosd(angle);

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

uint16_t distanceBetweenPoints(position_t* positions)
{
    float dx = positions[0].x - positions[1].x;
    float dy = positions[0].x - positions[1].y;
    return sqrt(dx*dx + dy*dy);
}

int16_t angleToTarget() {
    position_t current = getPosition();
    float dx = current_target.x - current.x;
    float dy = current_target.y - current.y;
    return atan2(dx, dy) * 180 / PI;
}

void setTarget(position_t candidate) {
    position_t actual;
    if (candidate.x < 0 + ARENA_BUFFER) {
        actual.x = 0 + ARENA_BUFFER;
    } else if (candidate.x > ARENA_WIDTH - ARENA_BUFFER) {
        actual.x = ARENA_WIDTH - ARENA_BUFFER;
    } else {
        actual.x = candidate.x;
    }

    if (candidate.y < 0 + ARENA_BUFFER) {
        actual.y = 0 + ARENA_BUFFER;
    } else if (candidate.y > ARENA_LENGTH - ARENA_BUFFER) {
        actual.y = ARENA_LENGTH - ARENA_BUFFER;
    } else {
        actual.y = candidate.y;
    }

    current_target = actual;
}

double cosd (double angle) {
    return cos(angle * PI / 180.0);
}

double sind (double angle) {
    return sin(angle * PI / 180.0);
}
