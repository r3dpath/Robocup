#include "positioning.h"


void positionTick()
{
    int32_t encChange = getEncoderDiff();
    uint16_t imuAngle = getIMUHeading();

    robotPosition.x += (float)encChange/12.6*cos((float)imuAngle*PI/180);
    robotPosition.y += (float)encChange/12.6*cos((float)imuAngle*PI/180);
}


void printPosition()
{
    Serial.print("P:");
    Serial.print(robotPosition.x);
    Serial.print(":");
    Serial.println(robotPosition.y);
}