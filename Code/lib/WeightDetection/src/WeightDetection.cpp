#include "WeightDetection.h"
#include "Movement.h"
#include "debug.h"
#include "IMU.h"

#define AVG_DEADBAND 1.5
#define ABS_DEADBAND 1.2
#define WASHOUT_RANGE 1

/*
TODO:
Should remain in collection state when really close


*/

// Constants for angular offsets
#define ANGLE_LEFT_CLOSE -10
#define ANGLE_LEFT_FAR -20
#define ANGLE_RIGHT_CLOSE 10
#define ANGLE_RIGHT_FAR 20
#define ANGLE_CENTER 0

extern TOF2 tof_scan_left;
extern TOF2 tof_scan_right;

weight_info_t weightDetection() {
    static weight_info_t state;
    int16_t max_left = 0;
    int16_t max_right = 0;
    uint8_t max_idx_left = 0; 
    uint8_t max_idx_right = 0;
    int16_t sum_left = 0;
    int16_t sum_right = 0;
    uint16_t max_diff = 0;
    uint16_t sum_top_distances = 0;
    int16_t detection_percentage = 0;

    // Collect TOF data for both left and right sensors
    for (int i = 0; i < 5; i++) {
        sum_left += tof_scan_left.differences[i];
        sum_right += tof_scan_right.differences[i];

        // Find max TOF difference on the left
        if (tof_scan_left.differences[i] > max_left) {
            max_left = tof_scan_left.differences[i];
            max_idx_left = i;
        }

        // Find max TOF difference on the right
        if (tof_scan_right.differences[i] > max_right) {
            max_right = tof_scan_right.differences[i];
            max_idx_right = i;
           
        }

        sum_top_distances += (tof_scan_left.top[i] + tof_scan_right.top[i]);
    }
    
    //Serial.print("Max index left"); Serial.println(max_idx_left);
    //Serial.print("Max index Right"); Serial.println(max_idx_right);

    max_diff = max(max_left, max_right);
    //Calculate averages for each side
    int16_t average_left = sum_left / 5;
    int16_t average_right = sum_right / 5;
    int16_t average_sum_top_distances = sum_top_distances / 5;

    if (average_sum_top_distances > 0) {

        detection_percentage =  (max_diff * 100) / average_sum_top_distances;
    } else {
        detection_percentage = 0;
    }

    // Serial.print("Max Diff: "); Serial.println(max_diff);
    // Serial.print("Avg Top Distances: "); Serial.println(average_sum_top_distances);
    // Serial.print("Detection Percentage: "); Serial.println(detection_percentage);

   // Determine the direction based on TOF data
    if (state.certainty < 3) {
        // If left TOF detects a weight
        if (max_left > abs(average_left)  && tof_scan_left.top[max_idx_left] > tof_scan_left.bottom[max_idx_left] && detection_percentage > 15) { //detection percentage greater than 15%
            
            state.certainty += 1;
            state.distance = tof_scan_left.bottom[max_idx_left];
            
            // Assign direction based on SPAD index
            if ((max_idx_left == 0) || (max_idx_left == 1)) state.direction = FAR_LEFT;
            else if ((max_idx_left == 2) || (max_idx_left == 3)) state.direction = LEFT;
            else if ((max_idx_left == 4) || (max_idx_right == 0)) state.direction = CENTER; //tof_scan_left.top[4] > tof_scan_left.bottom[4]) && (tof_scan_right.top[0] > tof_scan_right.bottom[0] (absolute centre)

        // If right TOF detects a weight
        } else if ((max_right > abs(average_right) && tof_scan_right.top[max_idx_right] > tof_scan_right.bottom[max_idx_right]) && detection_percentage > 15) {

            state.certainty += 1;
            state.distance = tof_scan_right.bottom[max_idx_right];

            // Assign direction based on SPAD index
            if ((max_idx_right == 4) || (max_idx_right == 3)) state.direction = FAR_RIGHT;
            else if ((max_idx_right == 2) || (max_idx_right == 1)) state.direction = RIGHT;
            else if ((max_idx_left == 4) || (max_idx_right == 0)) state.direction = CENTER;

        } else {
            state.certainty = 0;
            state.direction = UNDEFINED;
            state.distance = -1;
        }

    } else {
        //Similar logic when certainty >= 3 (more confident in detection)
        if ((max_left > abs(average_left)) && (tof_scan_left.top[max_idx_left] > tof_scan_left.bottom[max_idx_left]) && detection_percentage > 15) {
        if ((max_idx_left == 0) || (max_idx_left == 1)) {
            state.direction = FAR_LEFT;
        } else if ((max_idx_left == 2) || (max_idx_left == 3)) {
            state.direction = LEFT;
        } else if ((max_idx_left == 4) || (max_idx_right == 0)) state.direction = CENTER;
        state.distance = tof_scan_left.bottom[max_idx_left];
        } else if ((max_right > abs(average_right)) &&
                (tof_scan_right.top[max_idx_right] > tof_scan_right.bottom[max_idx_right]) && detection_percentage > 15) {
            if ((max_idx_right == 4) || (max_idx_right == 3)) {
                state.direction = FAR_RIGHT;
            } else if ((max_idx_right == 2) || (max_idx_right == 1)) {
                state.direction = RIGHT;
            } else if ((max_idx_left == 4) || (max_idx_right == 0)) state.direction = CENTER;
            state.distance = tof_scan_right.bottom[max_idx_right];
        } else {
            state.certainty = 0;
            state.direction = UNDEFINED;
            state.distance = -1;
        }
    }

    // Adjust heading based on detected direction and IMU data
    int16_t heading_adjustment = 0;
    switch (state.direction) {
        case FAR_LEFT:
            heading_adjustment = ANGLE_LEFT_FAR;
            break;
        case LEFT:
            heading_adjustment = ANGLE_LEFT_CLOSE;
            break;
        case FAR_RIGHT:
            heading_adjustment = ANGLE_RIGHT_FAR;
            break;
        case RIGHT:
            heading_adjustment = ANGLE_RIGHT_CLOSE;
            break;
        case CENTER:
            heading_adjustment = ANGLE_CENTER;
            break;
        default:
            break;
    }

    Serial.print("Adjustemnt angle"); Serial.println(heading_adjustment);
    Serial.print("Current weight Location: ");
    switch (state.direction) {
        case FAR_LEFT: Serial.println("FAR LEFT"); break;
        case LEFT: Serial.println("LEFT"); break;
        case CENTER: Serial.println("CENTRE"); break;
        case RIGHT: Serial.println("RIGHT"); break;
        case FAR_RIGHT: Serial.println("FAR_RIGHT"); break;
        case UNDEFINED: Serial.println("UNDEFINED"); break;
    }

    // Use IMU heading to correct the robot's direction
    //adjustHeading(heading_adjustment);

    #ifdef DEBUG
    Serial.print("Direction: ");
    Serial.print(state.direction); 
    Serial.print(" Distance: ");
    Serial.println(state.distance);
    #endif 

    #ifdef DEBUG
    Serial.print("S:");
    Serial.print(tof_scan_left.top[0]); Serial.print(":"); Serial.print(tof_scan_left.top[1]); Serial.print(":"); Serial.print(tof_scan_left.top[2]); Serial.print(":"); Serial.print(tof_scan_left.top[3]); Serial.print(":"); Serial.print(tof_scan_left.top[4]); Serial.print(":");
    Serial.print(tof_scan_left.bottom[0]); Serial.print(":"); Serial.print(tof_scan_left.bottom[1]); Serial.print(":"); Serial.print(tof_scan_left.bottom[2]); Serial.print(":"); Serial.print(tof_scan_left.bottom[3]); Serial.print(":"); Serial.print(tof_scan_left.bottom[4]); Serial.print(":");
    Serial.print(tof_scan_right.top[0]); Serial.print(":"); Serial.print(tof_scan_right.top[1]); Serial.print(":"); Serial.print(tof_scan_right.top[2]); Serial.print(":"); Serial.print(tof_scan_right.top[3]); Serial.print(":"); Serial.print(tof_scan_right.top[4]); Serial.print(":");
    Serial.print(tof_scan_right.bottom[0]); Serial.print(":"); Serial.print(tof_scan_right.bottom[1]); Serial.print(":"); Serial.print(tof_scan_right.bottom[2]); Serial.print(":"); Serial.print(tof_scan_right.bottom[3]); Serial.print(":"); Serial.print(tof_scan_right.bottom[4]); Serial.print(":");
    Serial.print(state.direction); Serial.print(":"); Serial.println(state.distance);
    #endif 

    return state;
}
