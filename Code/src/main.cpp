#include <Wire.h>
#include <VL53L0X.h>
#include <VL53L1X.h>
#include <SparkFunSX1509.h>
#include <Servo.h>
#include <Arduino.h>
#include <QuadEncoder.h>
#include "BNO055_support.h"
#include <TOF.h>
#include <TaskScheduler.h>

const byte SX1509_ADDRESS = 0x3F;
#define VL53L0X_ADDRESS_START 0x30
#define VL53L1X_ADDRESS_START 0x35
SX1509 io; // Create an SX1509 object to be used throughout

int distance = 0;
int strength = 0;
boolean receiveComplete = false;

bool Find_weight_flag = false;

TOF tof_l(L0, 0, 0x30, &io); // Left TOF
TOF tof_r(L0, 1, 0x31, &io); // Right TOF
TOF tof_NAV(L1, 2, 0x35, &io); // Back TOF
TOF tof_b(L1, 3, 0x36, &io); // Navigation
TOF tof_f(L1, 4, 0x37, &io);// Front TOF

void setup()
{

  Serial.begin(921600);
  Serial2.begin(921600);

  io.begin(SX1509_ADDRESS);

  Wire.begin();
  Wire.setClock(400000); // use 400 kHz I2C

  tof_f.disable();
  tof_b.disable();
  tof_l.disable();
  tof_r.disable();
  tof_NAV.disable();
  tof_f.init();
  tof_b.init();
  tof_l.init();
  tof_r.init();
  tof_NAV.init();
}

void getTFminiData(int* distance, int* strength, boolean* complete) {
  static char i = 0;
  char j = 0;
  int checksum = 0; 
  static int rx[9];
  while(Serial1.available()) {  
    rx[i] = Serial1.read();
    if(rx[0] != 0x59) {
      i = 0;
    } else if(i == 1 && rx[1] != 0x59) {
      i = 0;
    } else if(i == 8) {
      for(j = 0; j < 8; j++) {
        checksum += rx[j];
      }
      if(rx[8] == (checksum % 256)) {
        *distance = rx[2] + rx[3] * 256;
        *strength = rx[4] + rx[5] * 256;
        *complete = true;
      }
      i = 0;
      break;
    } else {
      i++;
    } 
  } 
}


void loop()
{

  static uint16_t front_scan[5];
  static uint16_t nav_scan[5];
  static uint16_t left;
  static uint16_t right;
  static uint16_t back;
  static int longrange;

  tof_f.scan(front_scan);
  tof_NAV.scan(nav_scan); 
  
  left = tof_l.read();
  right = tof_r.read();
  back = tof_b.read();
  getTFminiData(&longrange, &strength, &receiveComplete);

  if(receiveComplete) {
    receiveComplete = false;
    Serial2.println(longrange);
  }

  Serial2.print(front_scan[0]);
  Serial2.print(":");
  Serial2.print(front_scan[1]);
  Serial2.print(":");
  Serial2.print(front_scan[2]);
  Serial2.print(":");
  Serial2.print(front_scan[3]);
  Serial2.print(":");
  Serial2.print(front_scan[4]);
  Serial2.print(":");
  Serial2.print(nav_scan[0]);
  Serial2.print(":");
  Serial2.print(nav_scan[1]);
  Serial2.print(":");
  Serial2.print(nav_scan[2]);
  Serial2.print(":");
  Serial2.print(nav_scan[3]); 
  Serial2.print(":");
  Serial2.print(nav_scan[4]);
  Serial2.print(":");
  Serial2.print(left);
  Serial2.print(":");
  Serial2.print(right);
  Serial2.print(":");
  Serial2.println(back);
}
