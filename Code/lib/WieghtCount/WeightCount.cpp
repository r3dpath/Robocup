#include "WeightCount.h"
#define MAX_COUNT 3
#define DISTANCE_THRESHOLD 0 //distance where the ramp is fully loaded with weights
#define DEBOUNCE_DELAY 1000
#define WEIGHT_HEIGHT 70

TOF tof_back(L1, 1, 0x32, &io); // Back TOF
uint8_t WeightCount = 0;
bool Fully_Collected = false;

unsigned long lastDetectionTIme = 0;

int initialD()
{
    int initialDistance = tof_back.read();
    return initialDistance;
}
void CheckWeightCount()
{
    int No_weights_Distance = initialD();
    int Current_Distance = tof_back.read();
    Serial.println(tof_back.read());

    uint16_t height_difference =  No_weights_Distance - Current_Distance;

    uint16_t Calculated_weight_count = height_difference / WEIGHT_HEIGHT;

    if ((Calculated_weight_count > MAX_COUNT) && (millis() - lastDetectionTIme) > DEBOUNCE_DELAY) {
        
        WeightCount = MAX_COUNT;
        lastDetectionTIme = millis();
    }

    if (WeightCount == MAX_COUNT) {
       Serial.print("Fully loaded");
    } else {
        Serial.print("Weight Collected Total: ");
        Serial.println(WeightCount);
    }

}

uint8_t getWeightCount() {
    return WeightCount;
}