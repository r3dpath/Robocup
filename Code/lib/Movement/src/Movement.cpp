#include "Movement.h"
#include "TOF.h"
#include "debug.h"

SX1509 io;
const byte SX1509_ADDRESS = 0x3F;
// TOF sensor objects
TOF tof_l(L0, 2, 0x30, &io); // Left TOF
TOF tof_r(L0, 0, 0x31, &io); // Right TOF
TOF tof_b(L1, 1, 0x32, &io); // Back TOF
TOF2 tof_scan(4, 0x35, 3, 0x36, &io); // Both front facing TOF's

Servo motorLeft, motorRight;

void initMovement() {
    // Setup servo objects
    motorLeft.attach(28);
    motorRight.attach(29);

    io.begin(SX1509_ADDRESS);

    Wire.begin();
    Wire.setClock(400000); // use 400 kHz I2C

    tof_b.disable();
    tof_l.disable();
    tof_r.disable();
    tof_scan.disable();
    tof_b.init();
    tof_l.init();
    tof_r.init();
    tof_scan.init();
}

void movementController() {
    #ifdef PROFILING
    elapsedMicros time;
    #endif

    int back_TOF = tof_b.read();   // Back

    #ifdef PROFILING
    Serial.print(time);
    Serial.println(" - Back");
    time = 0;
    #endif

    uint16_t front_TOF = tof_scan.f_distance;  // Front

    #ifdef PROFILING
    Serial.print(time);
    Serial.println(" - Front");
    time = 0;
    #endif

    int Left_TOF = tof_l.read();   // Left side

    #ifdef PROFILING
    Serial.print(time);
    Serial.println(" - Left");
    time = 0;
    #endif

    int Right_TOF = tof_r.read();  // Right side

    #ifdef PROFILING
    Serial.print(time);
    Serial.println(" - Right");
    time = 0;
    #endif

    static bool left = false, right = false; // Retain state between function calls


    #ifdef DEBUG
    Serial.print("T:");
    Serial.print(Left_TOF);Serial.print(":");Serial.print(front_TOF);Serial.print(":");Serial.println(Right_TOF);
    #endif

    Forward();

    if ((Left_TOF <= 250) && !right) {
        RightTurn();
        right = true;
        left = false;
    } else if (Right_TOF <= 250 && !left) {
        LeftTurn();
        left = true;
        right = false;
    }

    if (!left && !right) {
        if (front_TOF <= 250) {
            Reverse();
            if (Left_TOF > Right_TOF) {
                LeftTurn();
            } else {
                RightTurn();
            }
        }
    } else  if (front_TOF >= 1000) {
            Forward();
        }

    //When multiple obstacles detected up front and need to reverse back and turn
    if (((Left_TOF <= 350) && (Right_TOF <= 350)) && (front_TOF <= 400)) { 
        Reverse();
        if (Left_TOF > Right_TOF) {
            LeftTurn();
        } else if (Right_TOF > Left_TOF) {
            RightTurn();
        }
    }
    //In the case of getting too close to the wall, 
    if (front_TOF <= 250) {
        if (Left_TOF > Right_TOF) {
            LeftTurn();
        } else {
            RightTurn();
        }
    }
}

void Stationary() {
    motorLeft.writeMicroseconds(PPM_STOP);
    motorRight.writeMicroseconds(PPM_STOP);
}
void Forward() {
    motorLeft.writeMicroseconds(PPM_STOP-SPEED_FAST);
    motorRight.writeMicroseconds(PPM_STOP+SPEED_FAST);
}

void Reverse() {
    motorLeft.writeMicroseconds(PPM_STOP+SPEED_FAST);
    motorRight.writeMicroseconds(PPM_STOP-SPEED_FAST);
}

void RightTurn() {
    motorLeft.writeMicroseconds(PPM_STOP-SPEED_FAST);
    motorRight.writeMicroseconds(PPM_STOP-SPEED_FAST);
}

void LeftTurn() {
    motorLeft.writeMicroseconds(PPM_STOP+SPEED_FAST);
    motorRight.writeMicroseconds(PPM_STOP+SPEED_FAST);
}

void SlowBackward() {
    motorLeft.writeMicroseconds(PPM_STOP+SPEED_SLOW);
    motorRight.writeMicroseconds(PPM_STOP-SPEED_SLOW);
}

void SlowForward() {
    motorLeft.writeMicroseconds(PPM_STOP-SPEED_SLOW);
    motorRight.writeMicroseconds(PPM_STOP+SPEED_SLOW);
}

void SlowLeft() {
    motorLeft.writeMicroseconds(PPM_STOP+SPEED_SLOW);
    motorRight.writeMicroseconds(PPM_STOP+SPEED_SLOW);
}

void SlowRight() {
    motorLeft.writeMicroseconds(PPM_STOP-SPEED_SLOW);
    motorRight.writeMicroseconds(PPM_STOP-SPEED_SLOW);
}
