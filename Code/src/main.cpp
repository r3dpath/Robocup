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
#include <Arduino.h>
#include <QuadEncoder.h>
#include "BNO055_support.h"
#include <TOF.h>

const byte SX1509_ADDRESS = 0x3F;
#define VL53L0X_ADDRESS_START 0x30
#define VL53L1X_ADDRESS_START 0x35

#define STOP_SPEED  1500
#define MOVE_SPEED  350
#define TURNING_RATIO  2/3

SX1509 io; // Create an SX1509 object to be used throughout

Servo myservoA,myservoB; //A left, B right

uint32_t mCurPosValueL;
uint32_t old_position = 0;
uint32_t mCurPosValueR;
uint32_t old_position1 = 0;
QuadEncoder encLeft(1, 2, 3, 0);  // Encoder on channel 1 of 4 available
                                 // Phase A (pin0), PhaseB(pin1), Pullups Req(0)
QuadEncoder encRight(2, 5, 4, 0);  // Encoder on channel 2 of 4 available
                                 //Phase A (pin2), PhaseB(pin3), Pullups Req(0)

struct bno055_t myBNO;
struct bno055_euler myEulerData; //Structure to hold the Euler data

unsigned long lastTime = 0;

int distance = 0;
int strength = 0;
boolean receiveComplete = false;

bool Find_weight_flag = false;

TOF tof_l(L0, 0, 0x30, &io); // Left TOF
TOF tof_r(L0, 1, 0x31, &io); // Right TOF
TOF tof_NAV(L1, 2, 0x35, &io); // Back TOF
TOF tof_b(L1, 3, 0x36, &io); // Navigation
TOF tof_f(L1, 4, 0x37, &io);// Front TOF

typedef enum {
  ROAMING,
  WEIGHT_FINDING,
  COLLECTING_WEIGHT,

} Robot_states;

//IR sensor
int IRsensorAddress = 0xB0;
//int IRsensorAddress = 0x58;
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

void setup()
{
  myservoA.attach(28);  
  myservoB.attach(29);  
  
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial2.begin(19200);

  io.begin(SX1509_ADDRESS);

  Wire.begin();
  Wire.setClock(400000); // use 400 kHz I2C

  //set up encoder
  QuadEncodersInit(encLeft, encRight);

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

  //IMU setup
  //Initialize I2C communication
  Wire1.begin();

  //Initialization of the BNO055
  BNO_Init(&myBNO); //Assigning the structure to hold information about the device

  //Configuration to NDoF mode
  bno055_set_operation_mode(OPERATION_MODE_NDOF);

  delay(1);

  //IR camera setup
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
}

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

void SlowBackward()
{
  myservoA.writeMicroseconds(1800);      
  myservoB.writeMicroseconds(1200);
}

void mangItBackward()
{
  myservoA.writeMicroseconds(2000);      
  myservoB.writeMicroseconds(1000);
}

void smallLeft()
{
  myservoA.writeMicroseconds(1750);      
  myservoB.writeMicroseconds(1750);
}
void smallRight()
{
  myservoA.writeMicroseconds(1150);      
  myservoB.writeMicroseconds(1150);
  
}
void MoveMent_Controller()
{
  // Read sensor values
  int sensorL1_2 = tof_b.read(); 
  int sensorL1_1 = tof_f.read(); 
  int sensorL0Distance1 = tof_l.read();  //Left side
  int sensorL0Distance2 = tof_r.read();  //Right side

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

  if (((sensorL0Distance1 <= 350) && (sensorL0Distance2 <= 350)) && (((sensorL1_1) <= 400) && ((sensorL1_2) > 1000))) // unstucking
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
  Serial.println(sensorL1_2);
}

void getTFminiData(int* distance, int* strength, boolean* complete) {
  static char i = 0;
  char j = 0;
  int checksum = 0; 
  static int rx[9];
  if(Serial1.available()) {  
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
    } else {
      i++;
    } 
  } 
}

void Weight_detection()
{
  static uint16_t front_scan[4];
  static uint16_t nav_scan[4];

  tof_f.scan(front_scan);
  tof_NAV.scan(nav_scan);

  uint16_t Further_Left_Lower = nav_scan[3];
  uint16_t Mid_Left_Lower = nav_scan[2];
  uint16_t Mid_Right_Lower = nav_scan[1]; 
  uint16_t Further_Right_Lower = nav_scan[0];

  uint16_t Further_Left_Upper = front_scan[3];
  uint16_t Mid_Left_Upper = front_scan[2];
  uint16_t Mid_Right_Upper = front_scan[1];
  uint16_t Further_Right_Upper = front_scan[0];
 
  getTFminiData(&distance, &strength, &receiveComplete);
  
  uint16_t AVG_Left_TOF = (Further_Left_Lower + Further_Left_Upper) / 2;
  uint16_t AVG_Right_TOF = (Further_Right_Lower + Further_Right_Upper) / 2;
  uint16_t AVG_Mid_TOF = ((Mid_Left_Upper + Mid_Left_Lower)/ 2) + ((Mid_Right_Upper + Mid_Right_Lower)/2);

  int16_t Diff_Left_TOF = Further_Left_Upper - Further_Left_Lower;
  int16_t Diff_Right_TOF = Further_Right_Upper - Further_Right_Lower;
  int16_t Diff_Mid_TOF_Left = Mid_Left_Upper - Mid_Left_Lower;
  int16_t Diff_Mid_TOF_Right = Mid_Right_Upper - Mid_Right_Lower;



  // Serial.print(AVG_Left_TOF);
  // Serial.print(" : ");
  // Serial.print(AVG_Mid_TOF);
  // Serial.print(" : ");
  // Serial.println(AVG_Right_TOF);

  Serial.print(Further_Left_Upper);
  Serial.print(" : ");
  Serial.print(Mid_Left_Upper);
  Serial.print(" : ");
  Serial.print(Mid_Right_Upper);
  Serial.print(" : ");
  Serial.println(Further_Right_Upper);

  Serial.print(Further_Left_Lower);
  Serial.print(" : ");
  Serial.print(Mid_Left_Lower);
  Serial.print(" : ");
  Serial.print(Mid_Right_Lower);
  Serial.print(" : ");
  Serial.println(Further_Right_Lower);

  Serial.print(Diff_Left_TOF);
  Serial.print(" : ");
  Serial.print(Diff_Mid_TOF_Left);
  Serial.print(" : ");
  Serial.print(Diff_Mid_TOF_Right);
  Serial.print(" : ");
  Serial.println(Diff_Right_TOF);

  // if ( (Further_Left_Lower < Further_Left_Upper) )
  // {
  //   Serial.print("Weight on the left\n");
  //   smallLeft();
  // }
  // if ( (Further_Right_Lower < Further_Right_Upper))
  // {
  //   Serial.print("Weight on the Right\n");
  //   smallRight();
  // }
  // if (((Mid_Left_Upper && Mid_Right_Upper) > (Mid_Left_Lower && Mid_Right_Lower)) && (strength > 100))
  // {
  //   Serial.print("Found Weight\n");
  //   Forward();
  //   Find_weight_flag = true;
  // } else {
  //   Find_weight_flag = false;
  // }

}

Robot_states Robot_State_Machine(Robot_states current_state)
{
  switch (current_state)
  {
    case ROAMING:
      /* MOVEMENT STUFF */
      MoveMent_Controller();
      if (Find_weight_flag == true)
      {
        return WEIGHT_FINDING;
      }
      break;
    
    case WEIGHT_FINDING:
      Weight_detection();
      if (Find_weight_flag == false)
      {
        return ROAMING;
      } else if (Find_weight_flag = true) {
        return COLLECTING_WEIGHT;
      }
      break;
    case COLLECTING_WEIGHT:
      myservoA.writeMicroseconds(1500);      
      myservoB.writeMicroseconds(1500);
      break;
  }

  return current_state;
  Serial.print(current_state);
}
void loop()
{

  
  Weight_detection();
  /* ENCODER STUFF */

  // mCurPosValueL = encLeft.read();

  // if(mCurPosValueL != old_position){
  //   /* Read the position values. */
  //   Serial.printf("Current position value1: %ld\r\n", mCurPosValueL);
  //   Serial.printf("Position differential value1: %d\r\n", (int16_t)encLeft.getHoldDifference());
  //   Serial.printf("Position HOLD revolution value1: %d\r\n", encLeft.getHoldRevolution());
  //   Serial.println();
  // }

  // old_position = mCurPosValueL;

  // mCurPosValueR = encRight.read();

  // if(mCurPosValueR != old_position1){
  //   /* Read the position values. */
  //   Serial.printf("Current position value2: %ld\r\n", mCurPosValueR);
  //   Serial.printf("Position differential value2: %d\r\n", (int16_t)encRight.getHoldDifference());
  //   Serial.printf("Position revolution value2: %d\r\n", encRight.getHoldRevolution());
  //   Serial.println();
  // }

  // old_position1 = mCurPosValueR;
  
  // static Robot_states state = ROAMING;
  // state = Robot_State_Machine(state);

  // Serial.print(state);
  // Serial.print(Find_weight_flag);
  



  /* IMU STUFF */

  // if ((millis() - lastTime) >= 100) //To stream at 10Hz without using additional timers
  // {
  //   lastTime = millis();

  //   bno055_read_euler_hrp(&myEulerData);			//Update Euler data into the structure

  //   // Serial.print("Time Stamp: ");				//To read out the Time Stamp
  //   // Serial.println(lastTime);

  //   // Serial.print("Heading(Yaw): ");				//To read out the Heading (Yaw)
  //   // Serial.println(float(myEulerData.h) / 16.00);		//Convert to degrees

  //   // Serial.print("Roll: ");					//To read out the Roll
  //   // Serial.println(float(myEulerData.r) / 16.00);		//Convert to degrees

  //   // Serial.print("Pitch: ");				//To read out the Pitch
  //   // Serial.println(float(myEulerData.p) / 16.00);		//Convert to degrees

  //   if ((float(myEulerData.r) / 16.00) < -20) {
  //     mangItBackward();
  //     delay(300);
  //   }
  //   if ((float(myEulerData.r) / 16.00) > 20) {
  //     Forward();
  //     delay(300);
  //   }

  //   if (((float(myEulerData.p) / 16.00) < -20) || ((float(myEulerData.p) / 16.00) > 20))
  //   {
  //     SlowBackward();
  //     delay(500);
  //   }
  //   Serial.println();					//Extra line to differentiate between packets
  // }

  // static uint16_t* distances; //For printing the scanned TOF
  // distances = tof_NAV.scan();
  // Serial.print(distances[0]);
  // Serial.print(":");
  // Serial.print(distances[1]);
  // Serial.print(":");
  // Serial.print(distances[2]);
  // Serial.print(":");
  // Serial.println(distances[3]);
  // delay(100);

  // getTFminiData(&distance, &strength, &receiveComplete);
  // if(receiveComplete) {
  //   receiveComplete = false;
  //   Serial.print(distance);
  //   Serial.print("cm\t");
  //   Serial.print("strength: ");
  //   Serial.println(strength);
  // }

  //**IR camera stuff**
  // ledState = !ledState;
  // if (ledState) { digitalWrite(ledPin,HIGH); } else { digitalWrite(ledPin,LOW); }

  // //IR sensor read
  // Wire.beginTransmission(slaveAddress);
  // Wire.write(0x36);
  // Wire.endTransmission();

  // Wire.requestFrom(slaveAddress, 16);        // Request the 2 byte heading (MSB comes first)
  // for (i=0;i<16;i++) { data_buf[i]=0; }
  // i=0;
  // while(Wire.available() && i < 16) {
  //     data_buf[i] = Wire.read();
  //     i++;
  // }

  // Ix[0] = data_buf[1];
  // Iy[0] = data_buf[2];
  // s   = data_buf[3];
  // Ix[0] += (s & 0x30) <<4;
  // Iy[0] += (s & 0xC0) <<2;

  // Ix[1] = data_buf[4];
  // Iy[1] = data_buf[5];
  // s   = data_buf[6];
  // Ix[1] += (s & 0x30) <<4;
  // Iy[1] += (s & 0xC0) <<2;

  // Ix[2] = data_buf[7];
  // Iy[2] = data_buf[8];
  // s   = data_buf[9];
  // Ix[2] += (s & 0x30) <<4;
  // Iy[2] += (s & 0xC0) <<2;

  // Ix[3] = data_buf[10];
  // Iy[3] = data_buf[11];
  // s   = data_buf[12];
  // Ix[3] += (s & 0x30) <<4;
  // Iy[3] += (s & 0xC0) <<2;

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
  // delay(15);

}
