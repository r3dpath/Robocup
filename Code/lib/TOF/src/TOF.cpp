#include "TOF.h"
#include <Arduino.h>

// Single TOF Sensor

TOF::TOF(TOFType type, uint8_t xshutPin, uint8_t address, SX1509* io)
    : type(type), xshutPin(xshutPin), address(address), io(io) {}

void TOF::disable() {
    io->pinMode(xshutPin, OUTPUT);
    io->digitalWrite(xshutPin, LOW);
    delay(10);
}

bool TOF::init() {
    // Enable the sensor
    io->digitalWrite(xshutPin, HIGH);
    delay(10);

    if (type == L0) {
        sensorL0.setTimeout(500);
        if (!sensorL0.init()) {
            Serial2.println("TOF Panic");
            return false;
        }
        sensorL0.setAddress(address);
        sensorL0.startContinuous(50);
    } else {
        sensorL1.setTimeout(500);
        sensorL1.setDistanceMode(VL53L1X::Medium);
        sensorL1.setMeasurementTimingBudget(75000);
        if (!sensorL1.init()) {
            Serial2.println("TOF Panic");
            return false;
        }
        sensorL1.setAddress(address);
        //sensorL1.startContinuous(50);
    }

    return true;
}

uint16_t TOF::read() {
    if (type == L0) {
        return sensorL0.readRangeContinuousMillimeters();
    } else {
        return sensorL1.readSingle();
    }
}

bool TOF::timeoutOccurred() {
    if (type == L0) {
        return sensorL0.timeoutOccurred();
    } else {
        return sensorL1.timeoutOccurred();
    }
}

void TOF::startContinuous(uint16_t period) {
    if (type == L0) {
        sensorL0.startContinuous(period);
    } else {
        sensorL1.startContinuous(period);
    }
}

void TOF::scan(uint16_t* distances) {
    static uint16_t spad_locations[5] = {150, 174, 198, 222, 246};
    if (type == L0) {
        return;
    } else {
        sensorL1.setROISize(4, 5);
        for (int i = 0; i < 5; i++) {
            sensorL1.setROICenter(spad_locations[i]);
            //delay(100);
            distances[i] = sensorL1.readSingle();
        }
        sensorL1.setROISize(16, 16);
        sensorL1.setROICenter(223);
    }
}


// TOF Pair

TOF2::TOF2(uint8_t xshutPin1, uint8_t address1, uint8_t xshutPin2, uint8_t address2, SX1509* io)
    : xshutPin1(xshutPin1), address1(address1), xshutPin2(xshutPin2), address2(address2), io(io) {}

void TOF2::disable() {
    io->pinMode(xshutPin1, OUTPUT);
    io->digitalWrite(xshutPin1, LOW);
    io->pinMode(xshutPin2, OUTPUT);
    io->digitalWrite(xshutPin2, LOW);
    delay(10);
}

bool TOF2::init() {
    // Enable top sensor
    io->digitalWrite(xshutPin1, HIGH);
    delay(10);
    sensor_top.setTimeout(500);
    if (!sensor_top.init()) {
        Serial2.println("TOF2 Panic 1");
        return false;
    }
    sensor_top.setDistanceMode(VL53L1X::Medium);
    sensor_top.setMeasurementTimingBudget(75000);
    sensor_top.setAddress(address1);
    // Enable bottom sensor
    io->digitalWrite(xshutPin2, HIGH);
    delay(10);
    sensor_bottom.setTimeout(500);
    if (!sensor_bottom.init()) {
        Serial2.println("TOF2 Panic 2");
        return false;
    }
    sensor_bottom.setDistanceMode(VL53L1X::Medium);
    sensor_bottom.setMeasurementTimingBudget(75000);
    sensor_bottom.setAddress(address2);
    // Set SPAD size
    sensor_top.setROISize(4, 5);
    sensor_bottom.setROISize(4, 5);
    return true;
}

// Scans through the 5 SPAD locations and records the difference between the top and bottom sensors. Non-blocking.
void TOF2::tick() {
    static const uint16_t spad_locations[5] = {150, 174, 198, 222, 246};
    static uint8_t iter = 0;
    uint32_t start = micros();

    
    // Non-blocking read, will give zero if no good
    if (sensor_top.dataReady() && sensor_bottom.dataReady()) {
        top[iter] = sensor_top.read(false);
        bottom[iter] = sensor_bottom.read(false);
    } else {
        top[iter] = -1;
        bottom[iter] = -1;
    }

    // Only record difference if out by more than 10%
    if (bottom[iter] > top[iter]*0.9) {
        differences[iter] = 0;
    } else {
        differences[iter] = top[iter] - bottom[iter];
    }
    
    // Record actual center distance for navigation
    if (iter == 2) {
        distance = top[iter];
    }

    // Increment SPAD
    iter++;
    if (iter == 5) {
        iter = 0;
    }

    // Set the next SPAD location and initiate read (non-blocking)
    sensor_top.setROICenter(spad_locations[iter]);
    sensor_bottom.setROICenter(spad_locations[iter]);
    sensor_top.readSingle(false);
    sensor_bottom.readSingle(false);
}

// Returns the heading to detected weight. 0 is 15 degrees, 1 is 7.5 degrees, 2 is 0 degrees, 3 is -7.5 degrees, 4 is -15 degrees.
// -1 is no weight detected.
void TOF2::weight(uint16_t* heading, uint16_t* distance) {
    uint16_t max = 0;
    uint8_t max_idx = 0;

    // Iterate through differences and find the largest
    for (int i = 0; i < 5; i++) {
        if (differences[i] > max) {
            max = differences[i];
            max_idx = i;
        }
    }

    // If the differences arn't all zero, return the index of the largest
    if (max != 0) {
        *heading = max_idx;
        *distance = bottom[max_idx];
    } else {
        *heading = -1;
        *distance = -1;
    }

}