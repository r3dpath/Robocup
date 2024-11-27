#include "Positioning.h"

bool POS_OOB = false;

position_t robotPosition;
position_t homePosition;

void initPositioning() {
    if (START_BASE == BASE_LEFT) {
        homePosition = {300, 300};
    } else {
        homePosition = {ARENA_WIDTH - 300, 300};
    }
    robotPosition = {homePosition.x, homePosition.y};
}


void positionTick()
{
    int32_t encChange = getPosEncoderDiff();
    uint16_t imuAngle = getBodyHeading();

    robotPosition.x += (float)encChange/50*sin((float)imuAngle*PI/180);
    robotPosition.y += (float)encChange/50*cos((float)imuAngle*PI/180);

    // If the robot leaves the bounds of the arena, hold it at the edge.
    if (robotPosition.x > ARENA_WIDTH-ARENA_BUFFER) {
        robotPosition.x = ARENA_WIDTH-ARENA_BUFFER;
        POS_OOB = true;
    } else if (robotPosition.x < 0+ARENA_BUFFER) {
        robotPosition.x = 0+ARENA_BUFFER;
        POS_OOB = true;
    }

    if (robotPosition.y > ARENA_LENGTH-ARENA_BUFFER) {
        robotPosition.y = ARENA_LENGTH-ARENA_BUFFER;
        POS_OOB = true;
    } else if (robotPosition.y < 0+ARENA_BUFFER) {
        robotPosition.y = 0+ARENA_BUFFER;
        POS_OOB = true;
    }
}

position_t getPosition()
{
    return robotPosition;
}

void setPosition(position_t pos)
{
    robotPosition = pos;
}


void printPosition()
{
    Serial.print("P:");
    Serial.print(robotPosition.x);
    Serial.print(":");
    Serial.println(robotPosition.y);
}