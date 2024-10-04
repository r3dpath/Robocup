#include "IMU.h"

struct bno055_t myBNO;
struct bno055_euler myEulerData;
unsigned char bno055_calib;


bool cal = true;

void initIMU()
{
    // Initialize IMU
    Wire1.begin();
    BNO_Init(&myBNO); // Initialize BNO055
    bno055_set_operation_mode(OPERATION_MODE_NDOF);
    if (cal == true) {
        bno055_write_sic_matrix_zero(eeprom_read_word((uint16_t*)0));
        bno055_write_sic_matrix_one(eeprom_read_word((uint16_t*)2));
        bno055_write_sic_matrix_two(eeprom_read_word((uint16_t*)4));
        bno055_write_sic_matrix_three(eeprom_read_word((uint16_t*)6));
        bno055_write_sic_matrix_four(eeprom_read_word((uint16_t*)8));
        bno055_write_sic_matrix_five(eeprom_read_word((uint16_t*)10));
        bno055_write_sic_matrix_six(eeprom_read_word((uint16_t*)12));
        bno055_write_sic_matrix_seven(eeprom_read_word((uint16_t*)14));
        bno055_write_sic_matrix_eight(eeprom_read_word((uint16_t*)16));
    }

}

void UpdateIMU() 
{
    bno055_read_euler_hrp(&myEulerData); // Euler data, pitch, roll and heading. On the robot negative pitch is when tilted up. Returns degrees*16
    #ifdef DEBUG_IMU
    bno055_get_syscalib_status(&bno055_calib);
    Serial.print("Roll: ");
    Serial.print(myEulerData.r);
    Serial.print(" Pitch: ");
    Serial.print(myEulerData.p);
    Serial.print(" Heading: ");
    Serial.println(myEulerData.h/16);
    Serial.print("Calibration status: ");
    Serial.println(bno055_calib);
    #endif
    if (myEulerData.r < -320) { // When over 20 degrees inclined
        Reverse();
        
    }
    if ((myEulerData.p < -320) || (myEulerData.p > 320)) { // Over 20 degrees rolled either way
        SlowBackward();
    }

}

uint16_t getIMUHeading() {
    uint16_t Heading =  myEulerData.h / 16;
    return Heading;
}

void adjustHeading(int detected_angle) {
    int current_heading = getIMUHeading();  // Get current IMU heading
    int target_heading = current_heading + detected_angle;

    // Normalize target heading to stay within 0-360 degrees
    if (target_heading < 0) {
        target_heading += 360;
    } else if (target_heading >= 360) {
        target_heading -= 360;
    }

    // Continuously adjust until the current heading matches the target heading
    while (current_heading != target_heading) {
        current_heading = getIMUHeading();  // Update current heading

        if (detected_angle < 0) {
            // Turn left
            LeftTurn();
        } else if (detected_angle > 0) {
            // Turn right
            RightTurn();
        }

        // Optionally include a small delay to avoid rapid polling
        delay(10);
    }

    // Once heading is matched, move forward
    Forward();
}

void calibrateIMU()
{
    bno055_get_syscalib_status(&bno055_calib);
    if ((bno055_calib == 3) && (cal == false)) {
        BNO055_S16 zero;
        BNO055_S16 one;
        BNO055_S16 two;
        BNO055_S16 three;
        BNO055_S16 four;
        BNO055_S16 five;
        BNO055_S16 six;
        BNO055_S16 seven;
        BNO055_S16 eight;
        bno055_read_sic_matrix_zero(&zero);
        bno055_read_sic_matrix_one(&one);
        bno055_read_sic_matrix_two(&two);
        bno055_read_sic_matrix_three(&three);
        bno055_read_sic_matrix_four(&four);
        bno055_read_sic_matrix_five(&five);
        bno055_read_sic_matrix_six(&six);
        bno055_read_sic_matrix_seven(&seven);
        bno055_read_sic_matrix_eight(&eight);
        eeprom_write_word((uint16_t*)0, zero);
        eeprom_write_word((uint16_t*)2, one);
        eeprom_write_word((uint16_t*)4, two);
        eeprom_write_word((uint16_t*)6, three);
        eeprom_write_word((uint16_t*)8, four);
        eeprom_write_word((uint16_t*)10, five);
        eeprom_write_word((uint16_t*)12, six);
        eeprom_write_word((uint16_t*)14, seven);
        eeprom_write_word((uint16_t*)16, eight);
        cal = true;
        Serial.println("-- CALIBRATED --");
    }
}
