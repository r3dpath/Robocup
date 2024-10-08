#include "TOF.h"
#include <Arduino.h>
#include "debug.h"

const byte SX1509_ADDRESS = 0x3F;

SX1509 io;
// TOF sensor objects
TOF tof_l(L0, 2, 0x30, &io); // Left TOF
TOF tof_r(L0, 0, 0x31, &io); // Right TOF
TOF tof_count(L0, 6, 0x37, &io);  //TOF for wieght count
//TOF tof_b(L1, 1, 0x32, &io); // Back TOF
TOF2 tof_scan_left(5, 0x36, 3, 0x34, &io); // Both front facing TOF's
TOF2 tof_scan_right(4, 0x35, 1, 0x32, &io);

void initTOF()
{   
    io.begin(SX1509_ADDRESS);

    //tof_b.disable();
    tof_l.disable();
    tof_r.disable();
    tof_count.disable();
    tof_scan_left.disable();
    tof_scan_right.disable();
    
    //tof_b.init();
    tof_l.init();
    tof_r.init();
    tof_count.init();
    tof_scan_left.init();
    tof_scan_right.init();
}
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
            Serial.println("TOF Panic");
            return false;
        }
        sensorL0.setAddress(address);
        sensorL0.setMeasurementTimingBudget(45000);
        sensorL0.startContinuous(0);
    } else {
        sensorL1.setTimeout(500);
        sensorL1.setDistanceMode(VL53L1X::Medium);
        sensorL1.setMeasurementTimingBudget(45000);
        if (!sensorL1.init()) {
            Serial.println("TOF Panic");
            return false;
        }
        sensorL1.setAddress(address);
        sensorL1.startContinuous(50);
    }

    return true;
}

void TOF::tick() {
    if (type == L0) {
        range = sensorL0.readRangeContinuousMillimeters();
    } else {
        range = sensorL1.readRangeContinuousMillimeters(false);
    }
}

uint16_t TOF::read() {
    return range;
}

bool TOF::timeoutOccurred() {
    if (type == L0) {
        return sensorL0.timeoutOccurred();
    } else {
        return sensorL1.timeoutOccurred();
    }
}

void TOF::startContinuous(uint16_t period = 50) {
    if (type == L0) {
        sensorL0.startContinuous(period);
    } else {
        sensorL1.startContinuous(period);
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
        Serial.println("TOF2 Panic 1");
        return false;
    }
    sensor_top.setDistanceMode(TOF_SCAN_RANGE);
    sensor_top.setMeasurementTimingBudget(TOF_SCAN_WINDOW);
    sensor_top.setAddress(address1);
    // Enable bottom sensor
    io->digitalWrite(xshutPin2, HIGH);
    delay(10);
    sensor_bottom.setTimeout(500);
    if (!sensor_bottom.init()) {
        Serial.println("TOF2 Panic 2");
        return false;
    }
    sensor_bottom.setDistanceMode(TOF_SCAN_RANGE);
    sensor_bottom.setMeasurementTimingBudget(TOF_SCAN_WINDOW);
    sensor_bottom.setAddress(address2);
    // Set SPAD size
    sensor_top.setROISize(4, 5);
    sensor_bottom.setROISize(4, 5);
    return true;
}

// Scans through the 5 SPAD locations and records the difference between the top and bottom sensors. Non-blocking.
void TOF2::tick() {
    static const uint16_t spad_locations[5] = {246, 222, 198, 174, 150}; //{246, 222, 198, 174, 150};
    static uint8_t iter = 0;
    static uint8_t spad_iter = 1;
    
    // Non-blocking read, will give zero if no good
    if (sensor_top.dataReady() && sensor_bottom.dataReady()) {
        top[iter] = sensor_top.read(false);
        top_status[iter] = sensor_top.ranging_data.range_status;
        bottom[iter] = sensor_bottom.read(false);
        bottom_status[iter] = sensor_bottom.ranging_data.range_status;
    } else { 
        top[iter] = -1;
        top_status[iter] = -1;
        bottom[iter] = -1;
        bottom_status[iter] = -1;
    }

    differences[iter] = top[iter] - (bottom[iter]+TOF_CONSTANT_OFFSET);
    
    // Record actual center distance for navigation
    if (iter == 2) {
        f_distance = top[iter];
    }


    // Increment SPAD
    iter++;
    if (iter == 5) {
        iter = 0;
    }

    spad_iter++;
    if (spad_iter == 5) {
        spad_iter = 0;
    }

    // Set the next SPAD location and initiate read (non-blocking)
    
    sensor_top.setROICenter(spad_locations[spad_iter]);
    sensor_bottom.setROICenter(spad_locations[spad_iter]);
    sensor_top.readSingle(false);
    sensor_bottom.readSingle(false);
}
