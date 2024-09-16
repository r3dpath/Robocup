#include "TOFControl.h"
#include "Movement.h"
#include "TOF.h"

SX1509 io;
const byte SX1509_ADDRESS = 0x3F;
// TOF sensor objects
TOF tof_l(L0, 0, 0x30, &io); // Left TOF
TOF tof_r(L0, 1, 0x31, &io); // Right TOF
TOF tof_b(L1, 3, 0x36, &io); // Back TOF
TOF2 tof_scan(4, 0x35, 2, 0x37, &io); // Both front facing TOF's

void setupTOF() {
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

void MoveMent_Controller() {
    elapsedMicros time;
    int back_TOF = tof_b.read();   // Back
    Serial2.print(time);
    Serial2.println(" - Back");
    time = 0;
    uint16_t front_TOF = tof_scan.top[2];  // Front
    Serial2.print(time);
    Serial2.println(" - Front");
    time = 0;
    int Left_TOF = tof_l.read();   // Left side
    Serial2.print(time);
    Serial2.println(" - Left");
    time = 0;
    int Right_TOF = tof_r.read();  // Right side
    Serial2.print(time);
    Serial2.println(" - Right");
    time = 0;

    static bool left = false, right = false; // Retain state between function calls
    static unsigned long lastTurnTime = 0;

    #ifdef DEBUG
    Serial2.print("Back: ");
    Serial2.print(back_TOF);
    Serial2.print(" Front: ");
    Serial2.print(front_TOF);
    Serial2.print(" Left: ");
    Serial2.print(Left_TOF);
    Serial2.print(" Right: ");
    Serial2.println(Right_TOF);
    #endif

    Forward();

    if(millis() - lastTurnTime > random(2000, 5000)) { //random timer trigger random between 2 and 5 seconds
        int randomTurn = random(0, 2); // random generation of number 
        if (randomTurn == 0) {
            LeftTurn();
            left = true;
            right = false;
        } else {
            RightTurn();
            left = false;
            right = true;
        }
        lastTurnTime = millis(); //Resets timer
    }

    if ((Left_TOF <= 250) && !right) {
        RightTurn();
        right = true;
        left = false;
        lastTurnTime = millis();
    } else if (Right_TOF <= 250 && !left) {
        LeftTurn();
        left = true;
        right = false;
        lastTurnTime = millis();
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
        lastTurnTime = millis();
    } else  if (front_TOF >= 1000) {
            Forward();
        }

    //When multiple obstacles detected up front and need to reverse back and turn
    if (((Left_TOF <= 350) && (Right_TOF <= 350)) && (front_TOF <= 400)) { 
        mangItBackward();
        if (Left_TOF > Right_TOF) {
            LeftTurn();
        } else if (Right_TOF > Left_TOF) {
            RightTurn();
        }
        lastTurnTime = millis();
    }
    //In the case of getting too close to the wall, 
    if (front_TOF <= 250) {
        if (Left_TOF > Right_TOF) {
            LeftTurn();
        } else {
            RightTurn();
        }
        lastTurnTime = millis();
    }
}
