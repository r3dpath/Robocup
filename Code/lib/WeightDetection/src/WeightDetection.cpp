#include "WeightDetection.h"
#include "Movement.h"
#include "debug.h"

#define AVG_DEADBAND 1.5
#define ABS_DEADBAND 1.2
#define WASHOUT_RANGE 1

/*
TODO:
Should remain in collection state when really close


*/


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
    
    for (int i = 0; i < 5; i++) {
        sum_left += tof_scan_left.differences[i];
        sum_right += tof_scan_right.differences[i];

        if (tof_scan_left.differences[i] > max_left) {
            max_left = tof_scan_left.differences[i];
            max_idx_left = i;
        }
        if (tof_scan_right.differences[i] > max_right) {
            max_right = tof_scan_right.differences[i];
            max_idx_right = i;
        }
        
    }

    int16_t average_left = sum_left/5;
    int16_t average_right = sum_right/5;

    if (state.certainty < 3) {
        if ((max_left > abs(average_left)*AVG_DEADBAND || tof_scan_left.bottom[max_idx_left] < WASHOUT_RANGE)  && tof_scan_left.top[max_idx_left] > tof_scan_left.bottom[max_idx_left] * ABS_DEADBAND) {
            state.certainty += 1;
            state.direction = (weight_direction_t)max_idx_left;
            state.distance = tof_scan_left.bottom[max_idx_left];
        } else {
            state.certainty = 0;
            state.direction = UNDEFINED;
            state.distance = -1;
        }
    } else {
        if ((max_left > abs(average_left)*AVG_DEADBAND || tof_scan_left.bottom[max_idx_left] < WASHOUT_RANGE)  && tof_scan_left.top[max_idx_left] > tof_scan_left.bottom[max_idx_left] * ABS_DEADBAND) {
            if (tof_scan_left.bottom[max_idx_left] < WASHOUT_RANGE) {
                state.direction = CENTER;
            } else {
                state.direction = (weight_direction_t)max_idx_left;
            }
            state.distance = tof_scan_left.bottom[max_idx_left];
        } else {
            state.certainty = 0;
            state.direction = UNDEFINED;
            state.distance = -1;
        }
    }


    if (state.certainty < 3) {
        if ((max_right > abs(average_right)*AVG_DEADBAND || tof_scan_right.bottom[max_idx_right] < WASHOUT_RANGE)  && tof_scan_right.top[max_idx_right] > tof_scan_right.bottom[max_idx_right] * ABS_DEADBAND) {
            state.certainty += 1;
            state.direction = (weight_direction_t)max_idx_right;
            state.distance = tof_scan_right.bottom[max_idx_right];
        } else {
            state.certainty = 0;
            state.direction = UNDEFINED;
            state.distance = -1;
        }
    } else {
        if ((max_right > abs(average_right)*AVG_DEADBAND || tof_scan_right.bottom[max_idx_right] < WASHOUT_RANGE)  && tof_scan_right.top[max_idx_right] > tof_scan_right.bottom[max_idx_right] * ABS_DEADBAND) {
            if (tof_scan_right.bottom[max_idx_right] < WASHOUT_RANGE) {
                state.direction = CENTER;
            } else {
                state.direction = (weight_direction_t)max_idx_right;
            }
            state.distance = tof_scan_right.bottom[max_idx_right];
        } else {
            state.certainty = 0;
            state.direction = UNDEFINED;
            state.distance = -1;
        }
    }

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
