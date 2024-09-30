#include "WeightCount.h"

TOF tof_b(L1, 1, 0x32, &io); // Back TOF

void WeightCount()
{
    int back_TOF = tof_b.read();   
    Serial.println(back_TOF);

}