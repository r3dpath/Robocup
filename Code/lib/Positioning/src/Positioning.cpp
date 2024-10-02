#include "Positioning.h"
#include "debug.h"


position_t robotPosition;


void positionTick()
{
    int32_t encChange = getEncoderDiff();
    uint16_t imuAngle = getIMUHeading();

    robotPosition.x += (float)encChange/1.26*cos((float)imuAngle*PI/180);
    robotPosition.y += (float)encChange/1.26*sin((float)imuAngle*PI/180);
}


void printPosition()
{
    Serial.print("P:");
    Serial.print(robotPosition.x);
    Serial.print(":");
    Serial.println(robotPosition.y);
}