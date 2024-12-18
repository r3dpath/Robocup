#include <Arduino.h>
#include <TaskScheduler.h>
#include "debug.h"
#include "Collection.h" // Get rid of this once fsm
//#include <WeightCount.h>
#include "Navigator.h"
#include "Movement.h"
#include "Encoder.h"
#include "TOF.h"
#include "WeightDetection.h"



//#define TOTAL_ROUND_TIME 2*60*1000
#define TOTAL_ROUND_TIME 100000000UL
elapsedMillis round_time = 0;
/*

TODO:



*/

extern TOF2 tof_scan_left;
extern TOF2 tof_scan_right;
extern TOF tof_l;
extern TOF tof_r;

void initTask();
void tof_scan_restart();

void tof_scan_time() {
  elapsedMicros time;
  time = 0;
  tof_scan_restart();
  Serial.print(time);
  Serial.println(" - TOF Scan Tick Task");
}

void nsm_time() {
  elapsedMicros time;
  time = 0;
  navigatorFSM();
  Serial.print(time);
  Serial.println(" - Nav FSM Task");
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

void movementController_time() {
  elapsedMicros time;
  time = 0;
  movementController();
  Serial.print(time);
  Serial.println(" - Movement Task");
}


void weightTask_time() {
  elapsedMicros time;
  time = 0;
  weightTask();
  Serial.print(time);
  Serial.println(" - Weight Task");
}

void bodgeWeightTick() {
  tof_l.tick();
  tof_r.tick();
}

Scheduler taskManager;
#ifndef PROFILING
//Task tScan(35, TASK_ONCE, []() { tof_scan.tick(); });
Task tScan(TOF_SCAN_PERIOD, TASK_ONCE, tof_scan_restart);
Task tSL(TOF_SCAN_PERIOD * 5, TASK_FOREVER, []() { tof_l.tick(); });
Task tSR(TOF_SCAN_PERIOD * 5, TASK_FOREVER, []() { tof_r.tick(); });
Task tNav(200, TASK_FOREVER, navigatorFSM);
Task tPos(15, TASK_FOREVER, positionTick);
Task tMove(50, TASK_FOREVER, movementController);
Task tWeightDetect(TOF_SCAN_PERIOD * 5, TASK_FOREVER, weightTask);
#ifdef DEBUG_POS
Task tPrintPos(200, TASK_FOREVER, printPosition);
#endif
#else
Task tScan(TOF_SCAN_PERIOD, TASK_ONCE, tof_scan_time);
Task tSL(TOF_SCAN_PERIOD * 5, TASK_FOREVER, []() { tof_l.tick(); });
Task tSR(TOF_SCAN_PERIOD * 5, TASK_FOREVER, []() { tof_r.tick(); });
Task tNav(200, TASK_FOREVER, nsm_time);
Task tPos(15, TASK_FOREVER, positionTick_time);
Task tMove(50, TASK_FOREVER, movementController_time);
Task tWeightDetect(TOF_SCAN_PERIOD * 5, TASK_FOREVER, weightTask_time);
#ifdef DEBUG_POS
Task tPrintPos(200, TASK_FOREVER, printPosition);
#endif
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
    initTOF();
    initMovement();
    initIMU();
    initCollection();
    initEncoder();
    initNavigator();
    initPositioning();
    

    // Initialize the task scheduler
    initTask();
    
}

void initTask() {  
  
  // This is a class/library function. Initialise the task scheduler
  taskManager.init();     
 
  // Add tasks to the scheduler
  taskManager.addTask(tScan);
  taskManager.addTask(tNav);
  taskManager.addTask(tPos);
  taskManager.addTask(tMove);
  taskManager.addTask(tWeightDetect);
  taskManager.addTask(tSL);
  taskManager.addTask(tSR);
  #ifdef DEBUG_POS
  taskManager.addTask(tPrintPos);
  #endif

  // Enable the tasks
  tScan.enable();
  tNav.enable();
  tPos.enable();
  tMove.enable();
  tWeightDetect.enable();
  tSL.enable();
  tSR.enable();
  #ifdef DEBUG_POS
  tPrintPos.enable();
  #endif

  Serial.println("Tasks have been initialised \n");
}

void loop() {
  taskManager.execute();
  if (round_time > TOTAL_ROUND_TIME) {
    Serial.print("!Round Over!");
    while (1) {}
  }
}