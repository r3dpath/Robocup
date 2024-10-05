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
    int16_t heading = getIMUHeading();
    // Get current speed
    float lspeed = getLeftEncoderSpeed();
    float rspeed = getRightEncoderSpeed();
    // Calculate difference between current and desired heading
    int16_t heading_diff = set_heading - heading;
    // Calculate difference between current and desired speed
    // int8_t speed_diff = set_speed - speed;

    // If more than 180 degrees, subtract 360
    if (heading_diff > 180) {
        heading_diff -= 360;
    } else if (heading_diff < -180) {
        heading_diff += 360;
    }

    #ifdef DEBUG_MOVEMENT
    Serial.print("Heading: ");
    Serial.print(heading);
    Serial.print(" SetHeading: ");
    Serial.print(set_heading);
    Serial.print(" HeadingDiff: ");
    Serial.println(heading_diff);
    #endif

    // Implement PI control to achieve the desired heading and speed

    // Really crappy bodge to overcome stall
    if (lspeed == 0 && set_speed != 0)
    {
        l_integral += 1;
    } else if (lspeed != 0) {
        l_integral = 0;
    }

    if (rspeed == 0 && set_speed != 0)
    {
        r_integral += 1;
    } else if (rspeed != 0) {
        r_integral = 0;
    }

    // Calculate the motor speeds
    int16_t left_speed = set_speed * MOVEMENT_P + set_speed * lspeed + MOVEMENT_HEADING_MULT * heading_diff;
    int16_t right_speed = set_speed  * MOVEMENT_P + set_speed * rspeed - MOVEMENT_HEADING_MULT * heading_diff;

    #ifdef DEBUG_MOVEMENT
    Serial.print("LeftActualSpeed: ");
    Serial.print(left_speed);
    Serial.print(" RightActualSpeed: ");
    Serial.println(right_speed);
    #endif


    // Bound speeds
    if (left_speed > SPEED_MAX)
    {
        left_speed = SPEED_MAX;
    }
    else if (left_speed < -SPEED_MAX)
    {
        left_speed = -SPEED_MAX;
    }

    if (right_speed > SPEED_MAX)
    {
        right_speed = SPEED_MAX;
    }
    else if (right_speed < -SPEED_MAX)
    {
        right_speed = -SPEED_MAX;
    }

    // Set the motor speeds. Left should be negative just bacause of the motor orientation.
    #ifdef DEBUG_MOVEMENT
    Serial.print("LeftSetSpeed: ");
    Serial.print(left_speed);
    Serial.print(" RightSetSpeed: ");
    Serial.println(right_speed);
    #endif

    motorLeft.writeMicroseconds(PPM_STOP - left_speed);
    motorRight.writeMicroseconds(PPM_STOP + right_speed);
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

