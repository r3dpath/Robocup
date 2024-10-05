#ifndef __TOF_H__
#define __TOF_H__

const byte SX1509_ADDRESS = 0x3F;
SX1509 io;

#include <Wire.h>
#include <VL53L0X.h>
#include <VL53L1X.h>
#include <SparkFunSX1509.h>
#include <debug.h>

#define TOF_SCAN_HEIGHT 4
#define TOF_CONSTANT_OFFSET 0
#define TOF_SCAN_PERIOD 45 // ms
#define TOF_SCAN_WINDOW 40 * 1000 // us
#define TOF_SCAN_RANGE VL53L1X::Long

// TOF sensor objects
TOF tof_l(L0, 2, 0x30, &io); // Left TOF
TOF tof_r(L0, 0, 0x31, &io); // Right TOF
//TOF tof_b(L1, 1, 0x32, &io); // Back TOF
TOF2 tof_scan_left(5, 0x36, 3, 0x34, &io); // Both front facing TOF's
TOF2 tof_scan_right(4, 0x35, 1, 0x32, &io);

enum TOFType {
    L0,
    L1
};

void initTOF();

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
    uint8_t top_status[5];
    uint16_t bottom[5];
    uint8_t bottom_status[5];
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