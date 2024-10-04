#include "StateMachine.h"
#include "WeightDetection.h"
#include "Movement.h"
#include "debug.h"
#include "Collection.h"
#include "WeightCount.h"

#define MAX_WEIGHT_COUNT 3

static Robot_states current_state = ROAMING;
unsigned long lastTurnTime = 0;

void transition(Robot_states newState) {
    current_state = newState;
    lastTurnTime = millis(); //after state change reset timer
}

void printCurrentState()
{
    #ifdef DEBUG
    Serial.print("Current state: ");
    switch (current_state) {
        case ROAMING: Serial.println("ROAMING"); break;
        case COLLECT_WEIGHT: Serial.println("COLLECTING_WEIGHT"); break;
        case RETURNING_BASE: Serial.println("RETURNING_BASE"); break;
        case RANDOM_WALK: Serial.println("RANDOM_WALK"); break;
        case PURSUE_WEIGHT: Serial.println("PURSUE_WEIGHT"); break;
    }
    #endif
}

void Robot_State_Machine() {
    weight_info_t state;

    switch (current_state) {
        case ROAMING: {
            state = weightDetection();
            if (state.certainty > 1) {
                transition(PURSUE_WEIGHT);
            } else {
                movementController();
                if (millis() - lastTurnTime > (unsigned long)random(8000, 12000)) {
                    transition(RANDOM_WALK);
                }   
            }
            break;
        }

        case RANDOM_WALK: {
            state = weightDetection();
            if (state.certainty > 1) {
                transition(PURSUE_WEIGHT);
            }
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

        // COLLECT_WEIGHT
        // Active when weight is directly infront of the robot, continues moving forward for at least two seconds.
        // Currently no obstical avoidance active
        // Turn 180 degrees after collection. This should change for competition.
        case COLLECT_WEIGHT: {
            collectionOn();
            SlowForward();
            state = weightDetection();
            

            if ((millis() - lastTurnTime) > 3500) { // Collection time has passed, return to roaming
                collectionOff();
                turn180();
                //CheckWeightCount(); // This updates the weight count

                if (getWeightCount() >= MAX_WEIGHT_COUNT) { // Directly check with updated count
                    transition(RETURNING_BASE);
                } else {
                transition(ROAMING);
                }
            } 
            break;
        }

        // PURSUE_WEIGHT
        // When a weight is detected by the TOF scan persue weight will try to place the weight into the center of the field of view and move towards it
        // TODO: Add some obstical avoidance
        case PURSUE_WEIGHT: {
            state = weightDetection();
            if (state.certainty == 0) {
                transition(ROAMING);
                break;
            }
            if (state.direction == CENTER) {
                if (state.direction < 300) {
                    transition(COLLECT_WEIGHT);
                } else {
                    SlowForward();
                }
            } else {
                if (state.direction == LEFT || state.direction == FAR_LEFT) {
                    ForwardLeft();
                } else {
                    ForwardRight();
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