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
    int back_TOF = tof_b.read();   // Back
    uint16_t front_TOF = tof_scan.top[2];  // Front
    int Left_TOF = tof_l.read();   // Left side
    int Right_TOF = tof_r.read();  // Right side

    static bool left = false, right = false; // Retain state between function calls

    Serial2.print("Back: ");
    Serial2.print(back_TOF);
    Serial2.print(" Front: ");
    Serial2.print(front_TOF);
    Serial2.print(" Left: ");
    Serial2.print(Left_TOF);
    Serial2.print(" Right: ");
    Serial2.println(Right_TOF);

    Forward();

    if ((Left_TOF <= 250) && !right) {
        RightTurn();
        right = true;
        left = false;
        right = false;
    }

    if (Right_TOF <= 250 && !left) {
        LeftTurn();
        left = true;
        right = false;
        left = false;
    }

    if (!left && !right) {
        if ((front_TOF <= 250) && (back_TOF >= 250)) {
            Backward();
            if (Left_TOF > Right_TOF) {
                LeftTurn();
            } else if (Right_TOF > Left_TOF) {
                RightTurn();
            }
        }

        if ((front_TOF >= 1000) && (back_TOF <= 200)) {
            Forward();
        }
    }

    if (((Left_TOF <= 350) && (Right_TOF <= 350)) && ((front_TOF <= 400) && (back_TOF > 1000))) {
        mangItBackward();
        if (Left_TOF > Right_TOF) {
            LeftTurn();
        } else if (Right_TOF > Left_TOF) {
            RightTurn();
        }
    }

    if ((front_TOF <= 300) && (back_TOF <= 300)) {
        if (Left_TOF > Right_TOF) {
            LeftTurn();
        } else if (Right_TOF > Left_TOF) {
            RightTurn();
        }
    }
}
