#include "IRSensor.h"

IRSensor::IRSensor(int address) {
    slaveAddress = address >> 1; // Convert to appropriate I2C address
}

void IRSensor::begin() {
    Serial.begin(19200);
    Wire.begin();
    // IR sensor initialize
    Write_2bytes(0x30, 0x01); delay(10);
    Write_2bytes(0x30, 0x08); delay(10);
    Write_2bytes(0x06, 0x90); delay(10);
    Write_2bytes(0x08, 0xC0); delay(10);
    Write_2bytes(0x1A, 0x40); delay(10);
    Write_2bytes(0x33, 0x33); delay(10);
    delay(100);
}

void IRSensor::update() {
    Wire.beginTransmission(slaveAddress);
    Wire.write(0x36);
    Wire.endTransmission();

    Wire.requestFrom(slaveAddress, 16);
    for (int i = 0; i < 16; i++) {
        data_buf[i] = (i < Wire.available()) ? Wire.read() : 0;
    }

    for (int i = 0; i < 4; i++) {
        Ix[i] = data_buf[1 + i * 4];
        Iy[i] = data_buf[2 + i * 4];
        int s = data_buf[3 + i * 4];
        Ix[i] += (s & 0x30) << 4;
        Iy[i] += (s & 0xC0) << 2;
    }
}

void IRSensor::printData() {
    for (int i = 0; i < 4; i++) {
        Serial.print(Ix[i]);
        Serial.print(",");
        Serial.print(Iy[i]);
        if (i < 3) Serial.print(",");
    }
    Serial.println();
}

int IRSensor::getX(int index) {
    return (index >= 0 && index < 4) ? Ix[index] : -1;
}

int IRSensor::getY(int index) {
    return (index >= 0 && index < 4) ? Iy[index] : -1;
}

bool IRSensor::isSourceDetected() {
    return Ix[0] > 0; // Adjust based on detection criteria
}

void IRSensor::Write_2bytes(byte d1, byte d2) {
    Wire.beginTransmission(slaveAddress);
    Wire.write(d1);
    Wire.write(d2);
    Wire.endTransmission();
}
