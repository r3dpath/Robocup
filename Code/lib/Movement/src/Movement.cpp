#include "Movement.h"

Servo motorLeft, motorRight;

// Heading from 0 to 360
int16_t set_heading = 0;
// Speed from -10 to 10
int8_t set_speed = 0;

int16_t l_integral = 0;
int16_t r_integral = 0;

void initMovement() {
    // Setup servo objects
    motorLeft.attach(PIN_LEFT_MOTOR);
    motorRight.attach(PIN_RIGHT_MOTOR);
}

void movementController() 
{
    // Get current heading
    int16_t heading = getBodyHeading();
    // Get current speed
    float lspeed = getLeftEncoderSpeed();
    float rspeed = getRightEncoderSpeed();

    // #ifdef DEBUG_MOVEMENT
    // Serial.print("LeftActualSpeed: ");
    // Serial.print(lspeed);
    // Serial.print(" RightActualSpeed: ");
    // Serial.println(rspeed);
    // #endif

    // Calculate difference between current and desired heading
    int16_t heading_diff = set_heading - heading;

    // Calculate difference between current and desired speed

    // If more than 180 degrees, subtract 360
    if (heading_diff > 180) {
        heading_diff -= 360;
    } else if (heading_diff < -180) {
        heading_diff += 360;
    }

    // #ifdef DEBUG_MOVEMENT
    // Serial.print("Heading: ");
    // Serial.print(heading);
    // Serial.print(" SetHeading: ");
    // Serial.print(set_heading);
    // Serial.print(" HeadingDiff: ");
    // Serial.println(heading_diff);
    // #endif

    int16_t left_set_speed = set_speed * MOVEMENT_P + MOVEMENT_HEADING_MULT * heading_diff;
    int16_t right_set_speed = set_speed * MOVEMENT_P - MOVEMENT_HEADING_MULT * heading_diff;

    // Really crappy bodge to overcome stall
    if (abs(lspeed) < 200 && set_speed != 0)
    {
        left_set_speed *= 2;
    } 

    if (abs(rspeed) < 200 && set_speed != 0)
    {
        right_set_speed *= 2;
    }

    //Using 6000 counts per second as max speed

    // #ifdef DEBUG_MOVEMENT
    // Serial.print("SetSpeedLeft: ");
    // Serial.print(left_set_speed);
    // Serial.print(" SetSpeedRight: ");
    // Serial.println(right_set_speed);
    // #endif

    // Bound speeds
    if (left_set_speed > SPEED_MAX)
    {
        left_set_speed = SPEED_MAX;
    }
    else if (left_set_speed < -SPEED_MAX)
    {
        left_set_speed = -SPEED_MAX;
    }

    if (right_set_speed > SPEED_MAX)
    {
        right_set_speed = SPEED_MAX;
    }
    else if (right_set_speed < -SPEED_MAX)
    {
        right_set_speed = -SPEED_MAX;
    }

    // Set the motor speeds. Left should be negative just bacause of the motor orientation.
    // #ifdef DEBUG_MOVEMENT
    // Serial.print("LeftCommandedSpeed: ");
    // Serial.print(left_set_speed);
    // Serial.print(" RightCommandedSpeed: ");
    // Serial.println(right_set_speed);
    // #endif

    motorLeft.writeMicroseconds(PPM_STOP - left_set_speed);
    motorRight.writeMicroseconds(PPM_STOP + right_set_speed);
}

void setMovementHeading(int16_t heading)
{
    set_heading = heading;
}

void incrementMovementHeading(int16_t heading)
{
    set_heading += heading;
    if (set_heading > 360)
    {
        set_heading -= 360;
    }
    else if (set_heading < 0)
    {
        set_heading += 360;
    }
}

int16_t getMovementHeading()
{
    return set_heading;
}

void setMovementSpeed(int8_t speed)
{
    if (speed > 10)
    {
        speed = 10;
    }
    else if (speed < -10)
    {
        speed = -10;
    }
    set_speed = speed;
}

int8_t getMovementSpeed()
{
    return set_speed;
}

int16_t getBodyHeading()
{
    int16_t heading = getIMUHeading() + START_ANGLE;
    if (heading >= 360) {
        heading -= 360;
    } else if (heading < 0) {
        heading += 360;
    }
    return heading;
}

