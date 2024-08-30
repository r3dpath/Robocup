#include "TOF.h"

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
            Serial.println("TOF Panic");
            return false;
        }
        sensorL0.setAddress(address);
        sensorL0.startContinuous(50);
    } else {
        sensorL1.setTimeout(500);
        sensorL1.setDistanceMode(VL53L1X::Medium);
        sensorL1.setMeasurementTimingBudget(75000);
        if (!sensorL1.init()) {
            Serial.println("TOF Panic");
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