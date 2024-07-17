#include <Arduino.h>
#include <QuadEncoder.h>

uint32_t mCurPosValueL;
uint32_t old_position = 0;
uint32_t mCurPosValueR;
uint32_t old_position1 = 0;
QuadEncoder encLeft(1, 2, 3, 0);  // Encoder on channel 1 of 4 available
                                 // Phase A (pin0), PhaseB(pin1), Pullups Req(0)
QuadEncoder encRight(2, 4, 5, 0);  // Encoder on channel 2 of 4 available
                                 //Phase A (pin2), PhaseB(pin3), Pullups Req(0)
                                 
void setup()
{
  while(!Serial && millis() < 4000);

  /* Initialize the ENC module. */
  QuadEncodersInit(encLeft, encRight);
}

void loop(){
  
  /* This read operation would capture all the position counter to responding hold registers. */
  mCurPosValueL = encLeft.read();

  if(mCurPosValueL != old_position){
    /* Read the position values. */
    Serial.printf("Current position value1: %ld\r\n", mCurPosValueL);
    Serial.printf("Position differential value1: %d\r\n", (int16_t)encLeft.getHoldDifference());
    Serial.printf("Position HOLD revolution value1: %d\r\n", encLeft.getHoldRevolution());
    Serial.println();
  }

  old_position = mCurPosValueL;

  mCurPosValueR = encRight.read();

  if(mCurPosValueR != old_position1){
    /* Read the position values. */
    Serial.printf("Current position value2: %ld\r\n", mCurPosValueR);
    Serial.printf("Position differential value2: %d\r\n", (int16_t)encRight.getHoldDifference());
    Serial.printf("Position revolution value2: %d\r\n", encRight.getHoldRevolution());
    Serial.println();
  }

  old_position1 = mCurPosValueR;
}