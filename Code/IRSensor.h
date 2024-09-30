#ifndef IRSENSOR_H
#define IRSENSOR_H

#include <Wire.h>

class IRSensor {
public:
    IRSensor(int address);
    void begin();
    void update();
    void printData();

    int getX(int index);
    int getY(int index);
    bool isSourceDetected();

private:
    int slaveAddress;
    byte data_buf[16];
    int Ix[4];
    int Iy[4];
    void Write_2bytes(byte d1, byte d2);
};

#endif // IRSENSOR_H
