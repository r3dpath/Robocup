#include "WeightDetection.h"
#include "Movement.h"

extern TOF2 tof_scan;

bool Find_weight_flag = false;
bool Found_weight_flag = false;

bool getFindWeightFlag() {
    uint16_t distance;
    uint16_t heading;

    tof_scan.weight(&heading, &distance);

    if (distance != -1) {
        Find_weight_flag = true;
    } else {
        Find_weight_flag = false;
    }
    return(Find_weight_flag);
}

bool getFoundWeightFlag() {
    static uint8_t debounce = 3; 
    uint16_t distance;
    uint16_t heading;

    tof_scan.weight(&heading, &distance);

    if (heading == 2) {
        if (debounce == 0) {
            Find_weight_flag = true;
            debounce = 3;
        } else {
            debounce -= 1;
        }
    } else {
        debounce = 3;
        Find_weight_flag = false;
    }
    return(Find_weight_flag);
}
