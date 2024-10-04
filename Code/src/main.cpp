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

void initTask();
void tof_scan_restart();
extern TOF2 tof_scan_left;
extern TOF2 tof_scan_right;

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

void positionTick_time() {
  elapsedMicros time;
  time = 0;
  positionTick();
  Serial.print(time);
  Serial.println(" - Pos Tick Task");
}

void tickEncoder_time() {
  elapsedMicros time;
  time = 0;
  tickEncoder();
  Serial.print(time);
  Serial.println(" - Encoder Tick Task");
}

Scheduler taskManager;
#ifndef PROFILING
//Task tScan(35, TASK_ONCE, []() { tof_scan.tick(); });
Task tScan(TOF_SCAN_PERIOD, TASK_ONCE, tof_scan_restart);
Task tStateMachine(TOF_SCAN_PERIOD*5, TASK_FOREVER, Robot_State_Machine);
Task tPos(100, TASK_FOREVER, positionTick);
#ifdef DEBUG_POS
Task tPrintPos(200, TASK_FOREVER, printPosition);
#endif
Task tTickEncoder(15, TASK_FOREVER, tickEncoder);
#else
Task tScan(60, TASK_FOREVER, tof_scan_time);
Task tStateMachine(300, TASK_FOREVER, rsm_time);
Task tIMU(100, TASK_FOREVER, UpdateIMU_time);
Task tPos(50, TASK_FOREVER, positionTick_time);
#ifdef DEBUG_POS
Task tPrintPos(200, TASK_FOREVER, printPosition);
#endif
Task tTickEncoder(20, TASK_FOREVER, tickEncoder_time);
#endif

void tof_scan_restart() {
  tof_scan_left.tick();
  tof_scan_right.tick();
  tScan.restartDelayed(TOF_SCAN_PERIOD);
}

void setup() {
  
    Serial.begin(BAUD);
    Wire.begin();
    Wire1.begin();
    Wire.setClock(400000UL);
    Wire1.setClock(400000UL);

    // Initialize TOF controller (includes IMU)
    init_TOF();
    initMovement();
    initIMU();
    initCollection();
    initEncoder();
    

    // Initialize the task scheduler
    initTask();
    
}

void initTask() {  
  
  // This is a class/library function. Initialise the task scheduler
  taskManager.init();     
 
  // Add tasks to the scheduler
  taskManager.addTask(tScan);
  taskManager.addTask(tStateMachine);
  taskManager.addTask(tPos);
  #ifdef DEBUG_POS
  taskManager.addTask(tPrintPos);
  #endif
  taskManager.addTask(tTickEncoder);

  // Enable the tasks
  tScan.enable();
  tStateMachine.enable();
  tPos.enable();
  #ifdef DEBUG_POS
  tPrintPos.enable();
  #endif
  tTickEncoder.enable();

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