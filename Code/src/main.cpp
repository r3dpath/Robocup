#include <Wire.h>
#include <VL53L0X.h>
#include <VL53L1X.h>
#include <SparkFunSX1509.h>
#include <Servo.h>
#include <Arduino.h>
#include <QuadEncoder.h>
#include "BNO055_support.h"
#include <TOF.h>
#include "Movement.h"
#include "TOFControl.h"
#include "WeightDetection.h"
#include "StateMachine.h"
#include "IMU.h"
#include <TaskScheduler.h>

void task_init();

Scheduler taskManager;
Task tScan(60, TASK_FOREVER, []() { tofScan().tick(); });
Task tStateMachine(500, TASK_FOREVER, Robot_State_Machine);
Task tIMU(100, TASK_FOREVER, UpdateIMU);

void setup() {
  
    Serial.begin(115200);
    Serial1.begin(115200);
    Serial2.begin(921600);

    // Initialize TOF controller (includes IMU)
    task_init();
    setupTOF();
    setupMovement();
    IMU_setup();
}

void task_init() {  
  
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
