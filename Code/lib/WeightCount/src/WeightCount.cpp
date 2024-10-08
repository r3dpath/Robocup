#include "WeightCount.h"
#include "debug.h"
#define MAX_COUNT 3
#define DISTANCE_THRESHOLD 0 //distance where the ramp is fully loaded with weights
#define DEBOUNCE_DELAY 1500 //1.5 seconds before coutning weight;

extern TOF tof_count;

uint8_t WeightCount = 0;
bool Fully_Collected = false;

unsigned long lastDetectionTIme = 0;
int weight_count_distance;


void CheckWeightCount()
{
    tof_count.tick();
    weight_count_distance = tof_count.read();
    if ((weight_count_distance < 80)) {
        WeightCount = MAX_COUNT;
        Serial.println("Fully loaded");
    } else {
        WeightCount = 0;
    }
}

uint8_t getWeightCount() {
    return WeightCount;
}