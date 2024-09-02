#include <Wire.h>
#include <VL53L0X.h>
#include <VL53L1X.h>
#include <SparkFunSX1509.h>
#include <Arduino.h>
#include <TOF.h>
#include <TaskScheduler.h>

void task_init();
void print_weight();

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
TOF tof_b(L1, 3, 0x36, &io); // Navigation
TOF2 tof_scan(2, 0x35, 4, 0x37, &io);

Scheduler taskManager;
Task tScan(200, TASK_FOREVER, []() { tof_scan.tick(); });
Task tPrint(500, TASK_FOREVER, &print_weight);

void setup()
{

  Serial.begin(921600);
  Serial2.begin(921600);

  io.begin(SX1509_ADDRESS);

  Wire.begin();
  Wire.setClock(400000); // use 400 kHz I2C

  tof_b.disable();
  tof_l.disable();
  tof_r.disable();
  tof_scan.init();
  tof_b.init();
  tof_l.init();
  tof_r.init();


  task_init();
}

void task_init() {  
  
  // This is a class/library function. Initialise the task scheduler
  taskManager.init();     
 
  // Add tasks to the scheduler
  taskManager.addTask(tScan);
  taskManager.addTask(tPrint);

  // Enable the tasks
  tScan.enable();
  tPrint.enable();

  Serial2.println("Tasks have been initialised \n");
}

void print_weight () {
  uint16_t distance;
  uint16_t heading;

  tof_scan.weight(&heading, &distance);
  // Serial2.print(tof_scan.top[4]);
  // Serial2.print(":");
  // Serial2.print(tof_scan.top[3]);
  // Serial2.print(":");
  // Serial2.print(tof_scan.top[2]);
  // Serial2.print(":");
  // Serial2.print(tof_scan.top[1]);
  // Serial2.print(":");
  // Serial2.print(tof_scan.top[0]);
  // Serial2.print(":");
  // Serial2.print(tof_scan.bottom[4]);
  // Serial2.print(":");
  // Serial2.print(tof_scan.bottom[3]);
  // Serial2.print(":");
  // Serial2.print(tof_scan.bottom[2]);
  // Serial2.print(":");
  // Serial2.print(tof_scan.bottom[1]);
  // Serial2.print(":");
  // Serial2.print(tof_scan.bottom[0]);
  // Serial2.print(":");
  // Serial2.print(heading);
  // Serial2.print(":");
  // Serial2.println(distance);
}


void loop()
{
  taskManager.execute();
}
