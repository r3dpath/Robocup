#include "StateMachine.h"
#include "WeightDetection.h"
#include "Movement.h"
#include "debug.h"
#include "Collection.h"
#include "IMU.h"

typedef enum {
  ROAMING,
  COLLECT_WEIGHT,
  RETURNING_BASE,
  RANDOM_WALK,
  PURSUE_WEIGHT
} Robot_states;

static Robot_states current_state = ROAMING;
unsigned long startTime;
uint8_t num_weight = 0;
unsigned long lastTurnTime = 0;

#define TIME_LIMIT 120000000;

void transition(Robot_states newState) {
    current_state = newState;
    lastTurnTime = millis();//after state change reset timer
}

void printCurrentState()
{
    Serial.print("Current state: ");
    switch (current_state) {
        case ROAMING: Serial.println("ROAMING"); break;
        case COLLECT_WEIGHT: Serial.println("COLLECTING_WEIGHT"); break;
        case RETURNING_BASE: Serial.println("RETURNING_BASE"); break;
        case RANDOM_WALK: Serial.println("RANDOM_WALK"); break;
        case PURSUE_WEIGHT: Serial.println("PURSUE_WEIGHT"); break;
    }
}

void TurnToHeading(uint16_t TargetHeading)
{
    uint16_t CurrentHeading = getIMUHeading();

    int16_t Heading_Difference = TargetHeading - CurrentHeading;

    if (Heading_Difference > 180) {
        Heading_Difference -= 360;
    } else if (Heading_Difference < -180) {
        Heading_Difference += 360;
    }

    if (Heading_Difference > 5) {
        RightTurn();
    } else if (Heading_Difference < -5) {
        LeftTurn();
    } else {
        Stationary();
    }
}

void Robot_State_Machine() {
    weight_info_t state;
    unsigned long elapsedTime = millis(); //Robot run time

    switch (current_state) {
        case ROAMING: {
            state = weightDetection();
            if (state.certainty > 1) {
                transition(PURSUE_WEIGHT);
            }
            // Stops the robot if timer is over 2 minutes
            else if (elapsedTime > 120000000) {
                Stationary();
                while(1)
                {
                    //gets it stuck in the loop stoping all other execution
                }
            } else {
                movementController();
                if (millis() - lastTurnTime > (unsigned long)random(8000, 12000)) {
                    transition(RANDOM_WALK);
                }   
            }
            break;
        }

        case RANDOM_WALK: {
            
            uint16_t currentHeading = getIMUHeading();

            uint16_t randomAngle = random(-45, 46);
            uint16_t newHeading = currentHeading + randomAngle;

            if (newHeading < 0) newHeading += 360;
            if (newHeading >= 360) newHeading -= 360;

            TurnToHeading(newHeading);

            // Stay in random walk mode for 2-5 seconds
            if (millis() - lastTurnTime > (unsigned long)random(2000, 3000)) {
                transition(ROAMING);
            }
            break;
        }

        case COLLECT_WEIGHT: {
            collectionOn();
            SlowForward();
            state = weightDetection();
            if (state.certainty == 0) {
                collectionOff();
                transition(ROAMING);
            }
            //num_weight += 1; // Increment weight count

            // if ((num_weight == 3) && (elapsedTime < 105000)) { // Return home if 3 weights collected and within time
            //     current_state = RETURNING_BASE;
            // } else {
            //     current_state = ROAMING;
            // }
            break;
        }

        case PURSUE_WEIGHT: {
            state = weightDetection();
            if (state.certainty == 0) {
                transition(ROAMING);
                break;
            }
            if (state.direction == CENTER) {
                if (state.direction < 400) {
                    transition(COLLECT_WEIGHT);
                } else {
                    SlowForward();
                }
            } else {
                if (state.direction == LEFT || state.direction == FAR_LEFT) {
                    SlowLeft();
                } else {
                    SlowRight();
                }
            }
            break;
        }
        case RETURNING_BASE: {
            //Do return to base stuff
            break;
        }
    }

    printCurrentState();
}