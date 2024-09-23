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

/*

TODO:
Sort out how the weight found system should work
Sort out a finalised weight detection algorithm
Work out how to do a PS/2 connection
Sort the IMU so it actually does something

*/

void initTask();
extern TOF2 tof_scan;

void tof_scan_time() {
  elapsedMicros time;
  time = 0;
  tof_scan.tick();
  Serial2.print(time);
  Serial2.println(" - TOF Scan Tick Task");
}

void rsm_time() {
  elapsedMicros time;
  time = 0;
  Robot_State_Machine();
  Serial2.print(time);
  Serial2.println(" - State Machine Task");
}

void UpdateIMU_time() {
  elapsedMicros time;
  time = 0;
  UpdateIMU();
  Serial2.print(time);
  Serial2.println(" - IMU Task");
}  

Scheduler taskManager;
#ifndef PROFILING
Task tScan(60, TASK_FOREVER, []() { tof_scan.tick(); });
Task tStateMachine(300, TASK_FOREVER, Robot_State_Machine);
Task tIMU(100, TASK_FOREVER, UpdateIMU);
#else
Task tScan(60, TASK_FOREVER, tof_scan_time);
Task tStateMachine(300, TASK_FOREVER, rsm_time);
Task tIMU(100, TASK_FOREVER, UpdateIMU_time);
#endif

void setup() {
  
    Serial.begin(115200);
    Serial2.begin(921600);

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

  Serial2.println("Tasks have been initialised \n");
}

void loop() {
  taskManager.execute();
}
