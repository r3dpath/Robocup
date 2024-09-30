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
#include "WieghtCount.h"

//#define TOTAL_ROUND_TIME 2*60*1000
#define TOTAL_ROUND_TIME -1
/*

TODO:



*/

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
#else
Task tScan(60, TASK_FOREVER, tof_scan_time);
Task tStateMachine(300, TASK_FOREVER, rsm_time);
Task tIMU(100, TASK_FOREVER, UpdateIMU_time);
#endif

void tof_scan_restart() {
  tof_scan.tick();
  tScan.restartDelayed(TOF_SCAN_PERIOD);
}

void setup() {
  
    Serial.begin(BAUD);

    // Initialize TOF controller (includes IMU)
    initMovement();
    initIMU();
    initCollection();

    // Initialize the task scheduler
    initTask();
}

void initTask() {  
  
  // This is a class/library function. Initialise the task scheduler
  taskManager.init();     
 
  // Add tasks to the scheduler
  taskManager.addTask(tScan);
  taskManager.addTask(tStateMachine);
  taskManager.addTask(tIMU);

  // Enable the tasks
  tScan.enable();
  tStateMachine.enable();
  tIMU.enable();

  Serial.println("Tasks have been initialised \n");
}

void loop() {
  static elapsedMillis round_time;
  taskManager.execute();
  if (round_time > TOTAL_ROUND_TIME) {
    Serial.print("!Round Over!");
    while (1) {}
  }
}