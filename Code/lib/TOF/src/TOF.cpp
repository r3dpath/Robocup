#include "TOF.h"

TOF::TOF(TOFType type, uint8_t xshutPin, uint8_t address, SX1509* io)
    : type(type), xshutPin(xshutPin), address(address), io(io) {}

bool TOF::init() {
    // Disable/reset the sensor
    io->pinMode(xshutPin, OUTPUT);
    io->digitalWrite(xshutPin, LOW);
    delay(10);
    
    // Enable the sensor
    io->digitalWrite(xshutPin, HIGH);
    delay(10);

    if (type == L0) {
        sensorL0.setTimeout(500);
        if (!sensorL0.init()) {
            return false;
        }
        sensorL0.setAddress(address);
        sensorL0.startContinuous();
    } else {
        sensorL1.setTimeout(500);
        if (!sensorL1.init()) {
            return false;
        }
        sensorL1.setAddress(address);
        sensorL1.startContinuous();
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
