#include "WeightDetection.h"
#include "Movement.h"
#include "debug.h"
#include "IMU.h"

#define TOF_SENSORS_COUNT 5


extern TOF2 tof_scan_left;
extern TOF2 tof_scan_right;

weight_info_t weight = {UNDEFINED, -1, 0};

void weightTask () {
    weight = weightDetection();
}

weight_info_t checkWeight(){
    if (weight.certainty > 0) {
        return weight;
    } else {  
        return (weight_info_t){UNDEFINED, -1, 0};  // Corrected the struct initialization
    }
}

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

    const int CONFIRMATION_THRESHOLD = 3;  // Number of consistent reads needed to confirm a weight
    static int consistent_detections = 0; // Tracks how many consecutive detections occurred

    // Collect TOF data for both left and right sensors
    for (int i = 0; i < TOF_SENSORS_COUNT; i++) {
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
    
    max_diff = max(max_left, max_right);
    
    // Calculate averages for each side
    int16_t average_left = sum_left / TOF_SENSORS_COUNT;
    int16_t average_right = sum_right / TOF_SENSORS_COUNT;
    int16_t average_sum_top_distances = sum_top_distances / TOF_SENSORS_COUNT;

    detection_percentage = (average_sum_top_distances > 0) ? (max_diff * 100) / average_sum_top_distances : 0; // detection percentage

    // Detect weight based on top-bottom sensor differences
    if (tof_scan_left.differences[max_idx_left] > tof_scan_right.differences[max_idx_right]) {
        // If left TOF detects a weight
        // Was : ((tof_scan_left.differences[max_idx_left] > MIN_VALID_DIFF) && (tof_scan_left.top[max_idx_left] > tof_scan_left.bottom[max_idx_left] * PERCENTAGE_THRESHOLD)) 
        // Rational for change: The term top > bottom * 1.15 does nothing and will always be true with a min diff of 400 for resonable ranges.
        // I think the sensitivity can be improved by setting a max detection range and decreasing the min diff. Percentage can be discarded.
        if ((tof_scan_left.differences[max_idx_left] > MIN_VALID_DIFF) && (tof_scan_left.bottom[max_idx_left] < MAX_DETECTION_RANGE))
        { 
            state.certainty += 1;
            state.distance = tof_scan_left.bottom[max_idx_left];
                
            // Assign direction based on SPAD index
            switch (max_idx_left) {
                case 0:
                    state.direction = FURTHER_LEFT;
                    break;
                case 1:
                    state.direction = FAR_LEFT;
                    break;
                case 2:
                    state.direction = LEFT;
                    break;
                case 3:
                    state.direction = CENTER_LEFT;
                    break;
                case 4:
                    state.direction = CENTER;
                    break;
                default:
                    state.direction = UNDEFINED;
                    break;
            }
        } 
        else 
        {
            state.certainty = 0;
            state.direction = UNDEFINED;
            state.distance = -1;
        }
    } else {
        // If right TOF detects a weight
        // Was : ((tof_scan_right.differences[max_idx_right] > MIN_VALID_DIFF) && (tof_scan_right.top[max_idx_right] > tof_scan_right.bottom[max_idx_right] * PERCENTAGE_THRESHOLD))
        if (tof_scan_right.differences[max_idx_right] > MIN_VALID_DIFF && (tof_scan_right.bottom[max_idx_right] < MAX_DETECTION_RANGE)) {
            
            state.certainty += 1;
            state.distance = tof_scan_right.bottom[max_idx_right];
            
            // Assign direction based on SPAD index
            switch (max_idx_right) {
                case 0:
                    state.direction = CENTER;
                    break;
                case 1:
                    state.direction = CENTER_RIGHT;
                    break;
                case 2:
                    state.direction = RIGHT;
                    break;
                case 3:
                    state.direction = FAR_RIGHT;
                    break;
                case 4:
                    state.direction = FURTHER_RIGHT;
                    break;
                default:
                    state.direction = UNDEFINED;
                    break;
            }

        } else {
            state.certainty = 0;
            state.direction = UNDEFINED;
            state.distance = -1;
        }
    } 

    // Serial.print("Max Diff right:");
    // Serial.println(tof_scan_right.differences[max_idx_right]);
    // Serial.print("Max Diff Left:");
    // Serial.println(tof_scan_left.differences[max_idx_left]);
    // Serial.print("Raw_max_scan_TOP_L:");
    // Serial.println(tof_scan_left.top[max_idx_left]);
    // Serial.print("Raw_max_scan_Bottom_L:");
    // Serial.println(tof_scan_left.bottom[max_idx_left] * 1.15);
    // Serial.print("Raw_max_scan_TOP_R:");
    // Serial.println(tof_scan_right.top[max_idx_right]);
    // Serial.print("Raw_max_scan_Bottom_R:");
    // Serial.println(tof_scan_right.bottom[max_idx_right] * 1.15);

    #ifdef DEBUG
    Serial.print("Direction: ");
    Serial.print(state.direction); 
    Serial.print(" Distance: ");
    Serial.println(state.distance);
    #endif 



    #ifdef DEBUG_TOF
    Serial.print("S:");
    Serial.print(tof_scan_left.top[0]); Serial.print(":"); Serial.print(tof_scan_left.top[1]); Serial.print(":"); Serial.print(tof_scan_left.top[2]); Serial.print(":"); Serial.print(tof_scan_left.top[3]); Serial.print(":"); Serial.print(tof_scan_left.top[4]); Serial.print(":");
    Serial.print(tof_scan_left.bottom[0]); Serial.print(":"); Serial.print(tof_scan_left.bottom[1]); Serial.print(":"); Serial.print(tof_scan_left.bottom[2]); Serial.print(":"); Serial.print(tof_scan_left.bottom[3]); Serial.print(":"); Serial.print(tof_scan_left.bottom[4]); Serial.print(":");
    Serial.print(tof_scan_right.top[0]); Serial.print(":"); Serial.print(tof_scan_right.top[1]); Serial.print(":"); Serial.print(tof_scan_right.top[2]); Serial.print(":"); Serial.print(tof_scan_right.top[3]); Serial.print(":"); Serial.print(tof_scan_right.top[4]); Serial.print(":");
    Serial.print(tof_scan_right.bottom[0]); Serial.print(":"); Serial.print(tof_scan_right.bottom[1]); Serial.print(":"); Serial.print(tof_scan_right.bottom[2]); Serial.print(":"); Serial.print(tof_scan_right.bottom[3]); Serial.print(":"); Serial.print(tof_scan_right.bottom[4]); Serial.print(":");
    Serial.print(state.direction); Serial.print(":"); Serial.println(state.distance);
    #else 
    if (state.certainty >= CONFIRMATION_THRESHOLD) {
        Serial.print("S:");
        Serial.print(tof_scan_left.top[0]); Serial.print(":"); Serial.print(tof_scan_left.top[1]); Serial.print(":"); Serial.print(tof_scan_left.top[2]); Serial.print(":"); Serial.print(tof_scan_left.top[3]); Serial.print(":"); Serial.print(tof_scan_left.top[4]); Serial.print(":");
        Serial.print(tof_scan_left.bottom[0]); Serial.print(":"); Serial.print(tof_scan_left.bottom[1]); Serial.print(":"); Serial.print(tof_scan_left.bottom[2]); Serial.print(":"); Serial.print(tof_scan_left.bottom[3]); Serial.print(":"); Serial.print(tof_scan_left.bottom[4]); Serial.print(":");
        Serial.print(tof_scan_right.top[0]); Serial.print(":"); Serial.print(tof_scan_right.top[1]); Serial.print(":"); Serial.print(tof_scan_right.top[2]); Serial.print(":"); Serial.print(tof_scan_right.top[3]); Serial.print(":"); Serial.print(tof_scan_right.top[4]); Serial.print(":");
        Serial.print(tof_scan_right.bottom[0]); Serial.print(":"); Serial.print(tof_scan_right.bottom[1]); Serial.print(":"); Serial.print(tof_scan_right.bottom[2]); Serial.print(":"); Serial.print(tof_scan_right.bottom[3]); Serial.print(":"); Serial.print(tof_scan_right.bottom[4]); Serial.print(":");
        Serial.print(state.direction); Serial.print(":"); Serial.println(state.distance);
    }
    #endif 

    return state;
}
