#ifndef __TOF_H__
#define __TOF_H__

#include <Wire.h>
#include <VL53L0X.h>
#include <VL53L1X.h>
#include <SparkFunSX1509.h>

#define TOF_SCAN_HEIGHT 4

enum TOFType {
    L0,
    L1
};

class TOF {
public:
    TOF(TOFType type, uint8_t xshutPin, uint8_t address, SX1509* io);
    bool init();
    void disable();
    uint16_t read();
    bool timeoutOccurred();
    void startContinuous(uint16_t period = 50);
    uint16_t* scan();
    uint16_t* scan_1();

private:
    TOFType type;
    uint8_t xshutPin;
    uint8_t address;
    SX1509* io;
    VL53L0X sensorL0;
    VL53L1X sensorL1;
};

#endif // __TOF_H__
