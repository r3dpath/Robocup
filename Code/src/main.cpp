#include <Wire.h>
#include <VL53L0X.h>
#include <VL53L1X.h>
#include <SparkFunSX1509.h>
#include <Servo.h>
#include <Arduino.h>
#include <QuadEncoder.h>
#include "BNO055_support.h"
#include <TOF.h>
#include <Movement.h>
#include <WeightDetection.h>
#include <StateMachine.h>
#include <IMU.h>
#include <TaskScheduler.h>
#include <Collection.h>
#include "debug.h"
#include <WeightCount.h>
#include "Encoder.h"
#include "Positioning.h"

//#define TOTAL_ROUND_TIME 2*60*1000
#define TOTAL_ROUND_TIME -1
/*

TODO:



*/

// ********************************
int IRsensorAddress = 0xB0;
// int IRsensorAddress = 0x58;
int slaveAddress;
int ledPin = 13;
boolean ledState = false;
byte data_buf[16];
int i;

int Ix[4];
int Iy[4];
int s;

void Write_2bytes(byte d1, byte d2)
{
    Wire.beginTransmission(slaveAddress);
    Wire.write(d1); Wire.write(d2);
    Wire.endTransmission();
}
// ****************************

void initTask();
void tof_scan_restart();
extern TOF2 tof_scan;

void tof_scan_time() {
  elapsedMicros time;
  time = 0;
  tof_scan_restart();
  Serial.print(time);
  Serial.println(" - TOF Scan Tick Task");
}

void rsm_time() {
  elapsedMicros time;
  time = 0;
  Robot_State_Machine();
  Serial.print(time);
  Serial.println(" - State Machine Task");
}

void UpdateIMU_time() {
  elapsedMicros time;
  time = 0;
  UpdateIMU();
  Serial.print(time);
  Serial.println(" - IMU Task");
}  

Scheduler taskManager;
#ifndef PROFILING
//Task tScan(35, TASK_ONCE, []() { tof_scan.tick(); });
Task tScan(TOF_SCAN_PERIOD, TASK_ONCE, tof_scan_restart);
Task tStateMachine(TOF_SCAN_PERIOD*5, TASK_FOREVER, Robot_State_Machine);
Task tIMU(100, TASK_FOREVER, UpdateIMU);
Task tPos(50, TASK_FOREVER, positionTick);
Task tPrintPos(200, TASK_FOREVER, printPosition);
#else
Task tScan(60, TASK_FOREVER, tof_scan_time);
Task tStateMachine(300, TASK_FOREVER, rsm_time);
Task tIMU(100, TASK_FOREVER, UpdateIMU_time);
Task tPos(50, TASK_FOREVER, positionTick);
#endif

void tof_scan_restart() {
  tof_scan.tick();
  tScan.restartDelayed(TOF_SCAN_PERIOD);
}

void setup() {
  
    Serial.begin(BAUD);
    Serial.println("Serial Begin");

    // Initialize TOF controller (includes IMU)
    initMovement();
    initIMU();
    initCollection();
    initEncoder();

    // Initialize the task scheduler
    initTask();
  
    // ******************************
    slaveAddress = IRsensorAddress >> 1;   // This results in 0x21 as the address to pass to TWI
    Serial.begin(19200);
    pinMode(ledPin, OUTPUT);      // Set the LED pin as output
    Wire.begin();
    // IR sensor initialize
    Write_2bytes(0x30,0x01); delay(10);
    Write_2bytes(0x30,0x08); delay(10);
    Write_2bytes(0x06,0x90); delay(10);
    Write_2bytes(0x08,0xC0); delay(10);
    Write_2bytes(0x1A,0x40); delay(10);
    Write_2bytes(0x33,0x33); delay(10);
    delay(100);
    //****************************************
    
}

void initTask() {  
  
  // This is a class/library function. Initialise the task scheduler
  taskManager.init();     
 
  // Add tasks to the scheduler
  taskManager.addTask(tScan);
  taskManager.addTask(tStateMachine);
  taskManager.addTask(tIMU);
  taskManager.addTask(tPos);
  //taskManager.addTask(tPrintPos);

  // Enable the tasks
  tScan.enable();
  tStateMachine.enable();
  tIMU.enable();
  tPos.enable();
  tPrintPos.enable();

  Serial.println("Tasks have been initialised \n");
}

void loop() {
  static elapsedMillis round_time;
  taskManager.execute();
  if (round_time > TOTAL_ROUND_TIME) {
    Serial.print("!Round Over!");
    while (1) {}
  }
  
  
  //*******************************************
  ledState = !ledState;
  if (ledState) { digitalWrite(ledPin,HIGH); } else { digitalWrite(ledPin,LOW); }

  //IR sensor read
  Wire.beginTransmission(slaveAddress);
  Wire.write(0x36);
  Wire.endTransmission();

  Wire.requestFrom(slaveAddress, 16);        // Request the 2 byte heading (MSB comes first)
  for (i=0;i<16;i++) { data_buf[i]=0; }
  i=0;
  while(Wire.available() && i < 16) {
      data_buf[i] = Wire.read();
      i++;
  }

  Ix[0] = data_buf[1];
  Iy[0] = data_buf[2];
  s   = data_buf[3];
  Ix[0] += (s & 0x30) <<4;
  Iy[0] += (s & 0xC0) <<2;

  Ix[1] = data_buf[4];
  Iy[1] = data_buf[5];
  s   = data_buf[6];
  Ix[1] += (s & 0x30) <<4;
  Iy[1] += (s & 0xC0) <<2;

  Ix[2] = data_buf[7];
  Iy[2] = data_buf[8];
  s   = data_buf[9];
  Ix[2] += (s & 0x30) <<4;
  Iy[2] += (s & 0xC0) <<2;

  Ix[3] = data_buf[10];
  Iy[3] = data_buf[11];
  s   = data_buf[12];
  Ix[3] += (s & 0x30) <<4;
  Iy[3] += (s & 0xC0) <<2;

  // for(i=0; i<4; i++)
  // {
  //   if (Ix[i] < 1000)
  //     Serial.print("");
  //   if (Ix[i] < 100)
  //     Serial.print("");
  //   if (Ix[i] < 10)
  //     Serial.print("");
  //   Serial.print( int(Ix[i]) );
  //   Serial.print(",");
  //   if (Iy[i] < 1000)
  //     Serial.print("");
  //   if (Iy[i] < 100)
  //     Serial.print("");
  //   if (Iy[i] < 10)
  //     Serial.print("");
  //   Serial.print( int(Iy[i]) );
  //   if (i<3)
  //     Serial.print(",");
  // }
  // Serial.println("");

  // NEW code middle = 1023 / 2 = 511.5   ||  middle is 400 to 600  || 

  if (Ix[0] != 1023) { // Check if it can see IR, if not rotate to find it
    if (Ix[0] > 400 && Ix[0] < 600) { // In the middle 
      Serial.print("Middle\n");
      Forward();
    } if (Ix[0] < 400 ) {// move to the right
      Serial.print("Left\n");
      RightTurn();
    } if (Ix[0] > 600 ) { // move to the left
      Serial.print("Right\n");
      LeftTurn();
    }
  } else {  // nothing found
    Serial.print("Nothing Found\n");
    Stationary();  
  }
  
  
  // if (Ix[0] != 1023) { // Check if it can see IR, if not rotate to find it
  //   if (Ix[0] > 400 && Ix[0] < 600)  // In the middle 
  //     Serial.print("Middle\n");
  //     Forward();
  //   if (Ix[0] < 400 ) // move to the right
  //     Serial.print("Left\n");
  //     RightTurn();
  //   if (Ix[0] > 600 )  // move to the left
  //     Serial.print("Right\n");
  //     LeftTurn();
  // else   // nothing found
  //     Serial.print("Nothing Found\n");
  //     Stationary();  
  // }            


  delay(15);

}