#include "WeightCount.h"
#define MAX_COUNT 3
#define DISTANCE_THRESHOLD 0 //distance where the ramp is fully loaded with weights
#define DEBOUNCE_DELAY 1500 //1.5 seconds before coutning weight;

extern TOF tof_count;

uint8_t WeightCount = 0;
bool Fully_Collected = false;

unsigned long lastDetectionTIme = 0;
int weight_count_distance = tof_count.read();



void CheckWeightCount()
{
    weight_count_distance = tof_count.read();
    //Serial.println(weight_count_distance);
    if ((weight_count_distance < 70) && (millis() - lastDetectionTIme) > DEBOUNCE_DELAY) {
        WeightCount = MAX_COUNT;
        Serial.println("Fully loaded");
        lastDetectionTIme = millis();
    }
}

uint8_t getWeightCount() {
    return WeightCount;
}