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

extern TOF3 tof_scan; // Change from TOF2 to TOF3 for triangular arrangement

weight_info_t weightDetection() {
    static weight_info_t state;
    int16_t max = 0;
    uint8_t max_idx = 0;
    int16_t sum = 0;
    int16_t bottom_avg = 0;

    // Calculate differences and average from the triangular setup
    for (int i = 0; i < 5; i++) {
        // Calculate the average of the two bottom sensors for each index
        bottom_avg = (tof_scan.bottom1[i] + tof_scan.bottom2[i]) / 2;

        // Compute the difference between the top sensor and the average bottom sensors
        tof_scan.differences[i] = tof_scan.top[i] - bottom_avg;

        // Sum up the differences for averaging
        sum += tof_scan.differences[i];

        // Track the maximum difference and its index
        if (tof_scan.differences[i] > max) {
            max = tof_scan.differences[i];
            max_idx = i;
        }
    }

    int16_t average = sum / 5;

    if (state.certainty < 3) {
        if ((max > abs(average) * AVG_DEADBAND || (tof_scan.bottom1[max_idx] < WASHOUT_RANGE && tof_scan.bottom2[max_idx] < WASHOUT_RANGE)) && 
            tof_scan.top[max_idx] > (bottom_avg * ABS_DEADBAND)) {
            state.certainty += 1;
            state.direction = (weight_direction_t)max_idx;
            state.distance = (tof_scan.bottom1[max_idx] + tof_scan.bottom2[max_idx]) / 2; // Average distance of the bottom sensors
        } else {
            state.certainty = 0;
            state.direction = UNDEFINED;
            state.distance = -1;
        }
    } else {
        if ((max > abs(average) * AVG_DEADBAND || (tof_scan.bottom1[max_idx] < WASHOUT_RANGE && tof_scan.bottom2[max_idx] < WASHOUT_RANGE)) && 
            tof_scan.top[max_idx] > (bottom_avg * ABS_DEADBAND)) {
            if (tof_scan.bottom1[max_idx] < WASHOUT_RANGE && tof_scan.bottom2[max_idx] < WASHOUT_RANGE) {
                state.direction = CENTER;
            } else {
                state.direction = (weight_direction_t)max_idx;
            }
            state.distance = (tof_scan.bottom1[max_idx] + tof_scan.bottom2[max_idx]) / 2; // Average distance of the bottom sensors
        } else {
            state.certainty = 0;
            state.direction = UNDEFINED;
            state.distance = -1;
        }
    }

    #ifdef DEBUG
    Serial.print("S:");
    Serial.print(tof_scan.top[0]); Serial.print(":"); Serial.print(tof_scan.top[1]); Serial.print(":");
    Serial.print(tof_scan.top[2]); Serial.print(":"); Serial.print(tof_scan.top[3]); Serial.print(":");
    Serial.print(tof_scan.top[4]); Serial.print(":");
    Serial.print(tof_scan.bottom1[0]); Serial.print(":"); Serial.print(tof_scan.bottom1[1]); Serial.print(":");
    Serial.print(tof_scan.bottom1[2]); Serial.print(":"); Serial.print(tof_scan.bottom1[3]); Serial.print(":");
    Serial.print(tof_scan.bottom1[4]); Serial.print(":");
    Serial.print(tof_scan.bottom2[0]); Serial.print(":"); Serial.print(tof_scan.bottom2[1]); Serial.print(":");
    Serial.print(tof_scan.bottom2[2]); Serial.print(":"); Serial.print(tof_scan.bottom2[3]); Serial.print(":");
    Serial.print(tof_scan.bottom2[4]); Serial.print(":");
    Serial.print(state.direction); Serial.print(":"); Serial.println(state.distance);
    #endif 

    return state;
}
