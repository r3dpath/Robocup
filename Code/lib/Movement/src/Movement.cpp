#include "Movement.h"
#include <Arduino.h>

// Servo objects
Servo myservoA, myservoB;

void setupMovement() {
    myservoA.attach(28);
    myservoB.attach(29);
}

void Forward() {
    myservoA.writeMicroseconds(1050);
    myservoB.writeMicroseconds(1950);
}

void Backward() {
    myservoA.writeMicroseconds(1950);
    myservoB.writeMicroseconds(1050);
}

void RightTurn() {
    myservoA.writeMicroseconds(1050);
    myservoB.writeMicroseconds(1050);
}

void LeftTurn() {
    myservoA.writeMicroseconds(1950);
    myservoB.writeMicroseconds(1950);
}

void SlowBackward() {
    myservoA.writeMicroseconds(1800);
    myservoB.writeMicroseconds(1200);
}

void SlowForward() {
    myservoA.writeMicroseconds(1800);
    myservoB.writeMicroseconds(1200);
}

void mangItBackward() {
    myservoA.writeMicroseconds(2000);
    myservoB.writeMicroseconds(1000);
}

void smallLeft() {
    myservoA.writeMicroseconds(1750);
    myservoB.writeMicroseconds(1750);
}

void smallRight() {
    myservoA.writeMicroseconds(1150);
    myservoB.writeMicroseconds(1150);
}

