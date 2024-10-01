#include "WeightCount.h"
#define MAX_COUNT 3
#define DISTANCE_THRESHOLD 0 //distance where the ramp is fully loaded with weights
#define DEBOUNCE_DELAY 1500

uint8_t WeightCount = 0;
bool Fully_Collected = false;

unsigned long lastDetectionTIme = 0;

void CheckWeightCount()
{
    //Serial.println(getBackTOFreading());
    if ((getBackTOFreading() < 60) && (millis() - lastDetectionTIme) > DEBOUNCE_DELAY) {
        WeightCount = MAX_COUNT;
        Serial.println("Fully loaded");
        lastDetectionTIme = millis();
    }
}

uint8_t getWeightCount() {
    return WeightCount;
}