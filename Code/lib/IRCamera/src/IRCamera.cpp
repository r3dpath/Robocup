#include "IRCamera.h"

IRSensor::IRSensor(int address) 
    : IRsensorAddress(address), ledPin(13), ledState(false) {
    slaveAddress = IRsensorAddress >> 1; // This results in 0x21 as the address to pass to TWI
}

void IRSensor::Write_2bytes(byte d1, byte d2) {
    Wire.beginTransmission(slaveAddress);
    Wire.write(d1);
    Wire.write(d2);
    Wire.endTransmission();
}

void IRSensor::setup() {
    Serial.begin(19200);
    pinMode(ledPin, OUTPUT); // Set the LED pin as output
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

uint16_t IRSensor::getPos() {
    // IR sensor read
    Wire.beginTransmission(slaveAddress);
    Wire.write(0x36);
    Wire.endTransmission();

    Wire.requestFrom(slaveAddress, 16); // Request the 2 byte heading (MSB comes first)
    for (int i = 0; i < 16; i++) {
        data_buf[i] = 0;
    }

    int i = 0;
    while (Wire.available() && i < 16) {
        data_buf[i] = Wire.read();
        i++;
    }

    for (int j = 0; j < 4; j++) {
        Ix[j] = data_buf[1 + j * 4];
        Iy[j] = data_buf[2 + j * 4];
        s = data_buf[3 + j * 4];
        Ix[j] += (s & 0x30) << 4;
        Iy[j] += (s & 0xC0) << 2;
    }

    return Ix[0];
    
}

  // // NEW code middle = 1023 / 2 = 511.5   ||  middle is 400 to 600  || 

  // if (Ix[0] != 1023) { // Check if it can see IR, if not rotate to find it
  //   if (Ix[0] > 400 && Ix[0] < 600) { // In the middle 
  //     Serial.print("Middle\n");
  //     Forward();
  //   } if (Ix[0] < 400 ) {// move to the right
  //     Serial.print("Left\n");
  //     RightTurn();
  //   } if (Ix[0] > 600 ) { // move to the left
  //     Serial.print("Right\n");
  //     LeftTurn();
  //   }
  // } else {  // nothing found
  //   Serial.print("Nothing Found\n");
  //   Stationary();  
  // }
