#ifndef IRSENSOR_H
#define IRSENSOR_H

#include <Wire.h>


class IRSensor {
public:
    IRSensor(int address);
    void setup();
    uint16_t getPos();

private:
    void Write_2bytes(byte d1, byte d2);
    
    int IRsensorAddress;
    int slaveAddress;
    int ledPin;
    bool ledState;
    byte data_buf[16];
    uint16_t Ix[4];
    uint16_t Iy[4];
    int s;
};

#endif // IRSENSOR_H


// #include "IRSensor.h"

// IRSensor irSensor(0xB0); // Replace with your desired sensor address

// void setup() {
//     irSensor.setup();
// }

// void loop() {
//     irSensor.loop();
// }