#ifndef __TOF_H__
#define __TOF_H__

#include <Wire.h>
#include <VL53L0X.h>
#include <VL53L1X.h>
#include <SparkFunSX1509.h>

#define TOF_SCAN_HEIGHT 4
#define TOF_CONSTANT_OFFSET 20

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

private:
    TOFType type;
    uint8_t xshutPin;
    uint8_t address;
    SX1509* io;
    VL53L0X sensorL0;
    VL53L1X sensorL1;
};

class TOF2 {
public:
    TOF2(uint8_t xshutPin1, uint8_t address1, uint8_t xshutPin2, uint8_t address2, SX1509* io);
    bool init();
    void disable();
    void tick();
    uint16_t top[5];
    uint16_t bottom[5];
    uint16_t f_distance;
    int16_t differences[5];

private:
    uint8_t xshutPin1;
    uint8_t address1;
    uint8_t xshutPin2;
    uint8_t address2;
    SX1509* io;
    VL53L1X sensor_top;
    VL53L1X sensor_bottom;
};

#endif // __TOF_H__