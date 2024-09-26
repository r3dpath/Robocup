#include "WeightDetection.h"
#include "Movement.h"
#include "debug.h"

#define AVG_DEADBAND 1.5
#define ABS_DEADBAND 1.1
#define WASHOUT_RANGE 300

extern TOF2 tof_scan;

weight_info_t weightDetection() {
    static weight_info_t state;
    int16_t max = 0;
    uint8_t max_idx = 0;
    int16_t sum = 0;
    
    for (int i = 0; i < 5; i++) {
        sum += tof_scan.differences[i];
        if (tof_scan.differences[i] > max) {
            max = tof_scan.differences[i];
            max_idx = i;
        }
    }

    int16_t average = sum/5;

    if (state.certainty < 3) {
        if ((max > abs(average)*AVG_DEADBAND || tof_scan.bottom[max_idx] < WASHOUT_RANGE)  && tof_scan.top[max_idx] > tof_scan.bottom[max_idx] * ABS_DEADBAND) {
            state.certainty += 1;
            state.direction = (weight_direction_t)max_idx;
            state.distance = tof_scan.bottom[max_idx];
        } else {
            state.certainty = 0;
            state.direction = UNDEFINED;
            state.distance = -1;
        }
    } else {
        if ((max > abs(average)*AVG_DEADBAND || tof_scan.bottom[max_idx] < WASHOUT_RANGE)  && tof_scan.top[max_idx] > tof_scan.bottom[max_idx] * ABS_DEADBAND) {
            state.direction = (weight_direction_t)max_idx;
            state.distance = tof_scan.bottom[max_idx];
        } else {
            state.certainty = 0;
            state.direction = UNDEFINED;
            state.distance = -1;
        }
    }

    #ifdef DEBUG
    Serial2.print("S:");
    Serial2.print(tof_scan.top[0]); Serial2.print(":"); Serial2.print(tof_scan.top[1]); Serial2.print(":"); Serial2.print(tof_scan.top[2]); Serial2.print(":"); Serial2.print(tof_scan.top[3]); Serial2.print(":"); Serial2.print(tof_scan.top[4]); Serial2.print(":");
    Serial2.print(tof_scan.bottom[0]); Serial2.print(":"); Serial2.print(tof_scan.bottom[1]); Serial2.print(":"); Serial2.print(tof_scan.bottom[2]); Serial2.print(":"); Serial2.print(tof_scan.bottom[3]); Serial2.print(":"); Serial2.print(tof_scan.bottom[4]); Serial2.print(":");
    Serial2.print(state.direction); Serial2.print(":"); Serial2.println(state.distance);
    #endif

    return state;
}
