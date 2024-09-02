#include "Movement.h"
#include "IMU.h"


struct bno055_t myBNO;
struct bno055_euler myEulerData;

unsigned long lastTime = 0;

void IMU_setup()
{
     // Initialize IMU
    Wire1.begin();
    BNO_Init(&myBNO); // Initialize BNO055
    bno055_set_operation_mode(OPERATION_MODE_NDOF);
}

void UpdateIMU() 
{
        bno055_read_euler_hrp(&myEulerData); // Update Euler data
        if ((float(myEulerData.r) / 16.00) < -20) {
            mangItBackward();
            
        }
        if (((float(myEulerData.p) / 16.00) < -20) || ((float(myEulerData.p) / 16.00) > 20)) {
            SlowBackward();
        }
}
