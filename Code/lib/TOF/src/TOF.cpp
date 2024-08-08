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
        if (!sensorL1.init()) {
            Serial.println("TOF Panic");
            return false;
        }
        sensorL1.setAddress(address);
        sensorL1.startContinuous(50);
    }
    return true;
}

uint16_t TOF::read() {
    if (type == L0) {
        return sensorL0.readRangeContinuousMillimeters();
    } else {
        return sensorL1.read();
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

uint16_t* TOF::scan() {
    static uint16_t spad_locations[4] = {150, 174, 206, 238};
    static uint16_t distances[4];
    if (type == L0) {
        return 0;
    } else {
        sensorL1.setROISize(5, 5);
        for (int i = 0; i < 4; i++) {
            sensorL1.setROICenter(spad_locations[i]);
            //delay(100);
            distances[i] = sensorL1.readSingle();
        }
        sensorL1.setROISize(16, 16);
        sensorL1.setROICenter(223);
    }

    return distances;
}

uint16_t* TOF::scan_1() {
    static uint16_t spad_locations[4] = {150, 174, 206, 238};
    static uint16_t distances[4];
    if (type == L0) {
        return 0;
    } else {
        sensorL1.setROISize(5, 5);
        for (int i = 0; i < 4; i++) {
            sensorL1.setROICenter(spad_locations[i]);
            //delay(100);
            distances[i] = sensorL1.readSingle();
        }
        sensorL1.setROISize(16, 16);
        sensorL1.setROICenter(223);
    }

    return distances;
}