/*
This example shows how to set up and read multiple VL53L1X sensors connected to
the same I2C bus. Each sensor needs to have its XSHUT pin connected to a
different Arduino pin, and you should change sensorCount and the xshutPins array
below to match your setup.

For more information, see ST's application note AN4846 ("Using multiple VL53L0X
in a single design"). The principles described there apply to the VL53L1X as
well.
*/

#include <Wire.h>
#include <VL53L0X.h>
#include <VL53L1X.h>
#include <SparkFunSX1509.h>
#include <Servo.h>

const byte SX1509_ADDRESS = 0x3F;
#define VL53L0X_ADDRESS_START 0x30
#define VL53L1X_ADDRESS_START 0x35

#define STOP_SPEED  1500
#define MOVE_SPEED  350
#define TURNING_RATIO  2/3

// The number of sensors in your system.
const uint8_t sensorCountL0 = 2;
const uint8_t sensorCountL1 = 2;

// The Arduino pin connected to the XSHUT pin of each sensor.
const uint8_t xshutPinsL0[8] = {0,1};
const uint8_t xshutPinsL1[8] = {2,3};

SX1509 io; // Create an SX1509 object to be used throughout
VL53L0X sensorsL0[sensorCountL0];
VL53L1X sensorsL1[sensorCountL1];

Servo myservoA,myservoB; //A left, B right

void setup()
{

  myservoA.attach(28);  
  myservoB.attach(29);  
  
  Serial.begin(115200);

  io.begin(SX1509_ADDRESS);

  Wire.begin();
  Wire.setClock(400000); // use 400 kHz I2C

  // Disable/reset all sensors by driving their XSHUT pins low.
  for (uint8_t i = 0; i < sensorCountL0; i++)
  {
    io.pinMode(xshutPinsL0[i], OUTPUT);
    io.digitalWrite(xshutPinsL0[i], LOW);
  }

  for (uint8_t i = 0; i < sensorCountL1; i++)
  {
    io.pinMode(xshutPinsL1[i], OUTPUT);
    io.digitalWrite(xshutPinsL1[i], LOW);
  }

  // L0 Enable, initialize, and start each sensor, one by one.
  for (uint8_t i = 0; i < sensorCountL0; i++)
  {
    // Stop driving this sensor's XSHUT low. This should allow the carrier
    // board to pull it high. (We do NOT want to drive XSHUT high since it is
    // not level shifted.) Then wait a bit for the sensor to start up.
    //pinMode(xshutPins[i], INPUT);
    io.digitalWrite(xshutPinsL0[i], HIGH);
    delay(10);

    sensorsL0[i].setTimeout(500);
    if (!sensorsL0[i].init())
    {
      Serial.print("Failed to detect and initialize sensor L0 ");
      Serial.println(i);
      while (1);
    }

    // Each sensor must have its address changed to a unique value other than
    // the default of 0x29 (except for the last one, which could be left at
    // the default). To make it simple, we'll just count up from 0x2A.
    sensorsL0[i].setAddress(VL53L0X_ADDRESS_START + i);

    sensorsL0[i].startContinuous(50);
  }

  // L1 Enable, initialize, and start each sensor, one by one.
  for (uint8_t i = 0; i < sensorCountL1; i++)
  {
    // Stop driving this sensor's XSHUT low. This should allow the carrier
    // board to pull it high. (We do NOT want to drive XSHUT high since it is
    // not level shifted.) Then wait a bit for the sensor to start up.
    //pinMode(xshutPins[i], INPUT);
    io.digitalWrite(xshutPinsL1[i], HIGH);
    delay(10);

    sensorsL1[i].setTimeout(500);
    if (!sensorsL1[i].init())
    {
      Serial.print("Failed to detect and initialize sensor L1 ");
      Serial.println(i);
      while (1);
    }

    // Each sensor must have its address changed to a unique value other than
    // the default of 0x29 (except for the last one, which could be left at
    // the default). To make it simple, we'll just count up from 0x2A.
    sensorsL1[i].setAddress(VL53L1X_ADDRESS_START + i);

    sensorsL1[i].startContinuous(50);
  }
}

// void Forward()
//     {
//   int sensorL1_1 = sensorsL1[0].read();
//   int sensorL1_2 = sensorsL1[1].read();
//   int Left_motor = 1200;
//   int Right_Motor = 1800;

//   if((sensorL1_1 > 1000) && (sensorL1_2 >= 500))
//   {
//     myservoA.writeMicroseconds(Left_motor);     
//     myservoB.writeMicroseconds(Right_Motor); 
//   } 
  
// }

// void Reverse()
// {
//   int sensorL1_1 = sensorsL1[0].read();
//   int sensorL1_2 = sensorsL1[1].read();
//   int Left_motor = 1800;
//   int Right_Motor = 1200;

//   if((sensorL1_1 < 200) && (sensorL1_2 >= 200))
//   {
//     myservoA.writeMicroseconds(Left_motor);     
//     myservoB.writeMicroseconds(Right_Motor); 
//   }     
  
// }
void Forward()
{
  myservoA.writeMicroseconds(1050);      
  myservoB.writeMicroseconds(1950);      
}

void Backward()
{
  myservoA.writeMicroseconds(1950);      
  myservoB.writeMicroseconds(1050);      
}

void RightTurn()
{
  myservoA.writeMicroseconds(1050);      
  myservoB.writeMicroseconds(1050);
}

void LeftTurn()
{
  myservoA.writeMicroseconds(1950);      
  myservoB.writeMicroseconds(1950);
}

void Stop()
{
  myservoA.writeMicroseconds(1500);      
  myservoB.writeMicroseconds(1500);
}

void mangItBackward()
{
  myservoA.writeMicroseconds(2000);      
  myservoB.writeMicroseconds(1000);
}
void MoveMent_Controller()
{
  // Read sensor values
  int sensorL1_1 = sensorsL1[0].read(); 
  int sensorL1_2 = sensorsL1[1].read(); 
  int sensorL0Distance1 = sensorsL0[0].readRangeContinuousMillimeters(); //Left side
  int sensorL0Distance2 = sensorsL0[1].readRangeContinuousMillimeters(); //Right side

  static bool left = false, right = false; // Using static to retain state between function calls

  // Assume Forward() is the default action
  Forward();

  if ((sensorL0Distance1 <= 250) && !right) // Check if close to an obstacle on the right
  {
    RightTurn();
    right = true;
    left = false; // Reset left turn flag
    delay(300); // Adding delay to allow the turn to complete
    right = false;
  }

  if (sensorL0Distance2 <= 250 && !left) // Check if close to an obstacle on the left
  {
    LeftTurn();
    left = true;
    right = false; // Reset right turn flag
    delay(300); // Adding delay to allow the turn to complete
    left = false;
  }

  if (!left && !right)
  {
    if ((sensorL1_1 <= 250) && (sensorL1_2 >= 250) ) // Check if there's an obstacle close in the back
    {
      Backward();
      delay(300); // Adding delay to allow the backward movement to complete
      if(sensorL0Distance1 > sensorL0Distance2)
      {
        LeftTurn();
        delay(300);
      }
      else if (sensorL0Distance2 > sensorL0Distance1)
      {
        RightTurn();
        delay(300);
      }

    }

    if ((sensorL1_1 >= 1000) && (sensorL1_2 <= 200)) // Check if clear to move forward
    {
      Forward();
    }
  }

  if (((sensorL0Distance1 <= 350) && (sensorL0Distance2 <= 350)) && (((sensorL1_1) <= 400) && ((sensorL1_2) > 1000)))
  {
    mangItBackward();
    delay(1300);
    if(sensorL0Distance1 > sensorL0Distance2)
    {
      LeftTurn();
      delay(500);
    }
    else if (sensorL0Distance2 > sensorL0Distance1)
    {
      RightTurn();
      delay(500);
    }
    
  }

  if (((sensorL1_1) <= 300) && ((sensorL1_2 <= 300)))
  {
    if(sensorL0Distance1 > sensorL0Distance2)
    {
      LeftTurn();
      delay(300);
    }
    else if (sensorL0Distance2 > sensorL0Distance1)
    {
      RightTurn();
      delay(300);
    }
  }
  
}


void loop()
{
  for (uint8_t i = 0; i < sensorCountL0; i++)
  {
    Serial.print(sensorsL0[i].readRangeContinuousMillimeters());
    if (sensorsL0[i].timeoutOccurred()) { Serial.print(" TIMEOUT"); }
    Serial.print('\t');
  }

  for (uint8_t i = 0; i < sensorCountL1; i++)
  {
    Serial.print(sensorsL1[i].read());
    if (sensorsL1[i].timeoutOccurred()) { Serial.print(" TIMEOUT"); }
    Serial.print('\t');
  }
  
  Serial.println();
  MoveMent_Controller();

}
