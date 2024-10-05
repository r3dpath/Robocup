#include "Positioning.h"
#include "debug.h"


position_t robotPosition;


void positionTick()
{
    int32_t encChange = getPosEncoderDiff();
    uint16_t imuAngle = getIMUHeading();

    robotPosition.x += (float)encChange/50000*cos((float)imuAngle*PI/180);
    robotPosition.y += (float)encChange/50000*sin((float)imuAngle*PI/180);
}


void printPosition()
{
    Serial.print("P:");
    Serial.print(robotPosition.x);
    Serial.print(":");
    Serial.println(robotPosition.y);
}