#include "Movement.h"


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


    #ifdef DEBUG
    Serial.print("T:");
    Serial.print(Left_TOF);Serial.print(":");Serial.print(front_TOF);Serial.print(":");Serial.println(Right_TOF);
    #endif

    if (((Left_TOF <= 350) && (Right_TOF <= 350)) && (front_TOF <= 400)) { 
        Reverse();
        delay(1000); // BAD! Get rid of this
    }

    if ((Left_TOF <= MOV_MIN_DISTANCE)) {
        SlowRight();
    } else if (Right_TOF <= MOV_MIN_DISTANCE) {
        SlowLeft();
    } else if (front_TOF <= 250) {     // TODO: Need something more here to avoid ramps. Could be pretty tricky
        Reverse();
        int16_t heading;
        if (Left_TOF < Right_TOF) {
            uint16_t CurrentHeading = getIMUHeading();
            heading = CurrentHeading + 90;
            if (heading > 360) {
                heading -= 360;
            }
        } else {
            uint16_t CurrentHeading = getIMUHeading();
            heading = CurrentHeading - 90;
            if (heading < 0) {
                heading += 360;
            }
        }
        elapsedMillis time = 0;
        while (!TurnToHeading(heading) && time<500) { // Seems like a shitty bodge too
            delay(1);
        }
    } else {
        Forward();
    }
}

bool TurnToHeading(uint16_t TargetHeading) // Returns true once rotated to align with heading
{
    uint16_t CurrentHeading = getIMUHeading();

    int16_t Heading_Difference = TargetHeading - CurrentHeading;

    if (Heading_Difference > 180) {
        Heading_Difference -= 360;
    } else if (Heading_Difference < -180) {
        Heading_Difference += 360;
    }

    if (Heading_Difference > 10) {
        SlowRight();
    } else if (Heading_Difference < -10) {
        SlowLeft();
    } else {
        return true;
    }
    return false;
}

void turn180() {
    uint16_t CurrentHeading = getIMUHeading();
    int16_t Heading_Difference = CurrentHeading - 180;
    if (Heading_Difference > 180) {
        Heading_Difference -= 360;
    } else if (Heading_Difference < -180) {
        Heading_Difference += 360;
    }
    elapsedMillis time = 0;
    while (time < 1000) {
        if (Heading_Difference > 5) {
            SlowRight();
        } else if (Heading_Difference < -5) {
            SlowLeft();
        } else {
            return;
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

void ForwardLeft() {
    motorLeft.writeMicroseconds(PPM_STOP+SPEED_SLOW);
    motorRight.writeMicroseconds(PPM_STOP+SPEED_SLOW-50);
}

void ForwardRight() {
    motorLeft.writeMicroseconds(PPM_STOP-SPEED_SLOW+50);
    motorRight.writeMicroseconds(PPM_STOP-SPEED_SLOW);
}

