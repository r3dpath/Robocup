#include "StateMachine.h"
#include "WeightDetection.h"
#include "Movement.h"
#include "debug.h"
#include "Collection.h"

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

void Robot_State_Machine() {
    weight_info_t state;
    unsigned long elapsedTime = millis(); //Robot run time

    switch (current_state) {
        case ROAMING: {
            state = weightDetection();
            if (state.certainty > 1) {
                current_state = PURSUE_WEIGHT;
            }
            // Stops the robot if timer is over 2 minutes
            else if (elapsedTime > 120000) {
                Stationary();
                while(1)
                {
                    //gets it stuck in the loop stoping all other execution
                }
            } else {
                movementController();
                if (millis() - lastTurnTime > (unsigned long)random(10000, 20000)) {
                    current_state = RANDOM_WALK;
                    lastTurnTime = millis();  // Reset the timer for random walk duration
                }
            }
            break;
        }

        case RANDOM_WALK: {
            // Randomly pick direction (0 for left, 1 for right) and turn type (0 for small, 1 for large)
            int randomTurn = random(0, 2);
            state = weightDetection();
            if (state.certainty > 1) {
                current_state = PURSUE_WEIGHT;
            }
            if (randomTurn == 0) {
                SlowLeft();
            } else {
                SlowRight(); 
            }

            // Stay in random walk mode for 2-5 seconds
            if (millis() - lastTurnTime > (unsigned long)random(1000, 2000)) {
                current_state = ROAMING;
                lastTurnTime = millis();  // Reset timer for roaming
            }
            break;
        }

        case COLLECT_WEIGHT: {
            collectionOn();
            SlowForward();
            state = weightDetection();
            if (state.certainty == 0) {
                collectionOff();
                current_state = ROAMING;
            }
            //num_weight += 1; // Increment weight count

            // if ((num_weight == 3) && (elapsedTime < 105000)) { // Return home if 3 weights collected and within time
            //     current_state = RETURNING_BASE;
            // } else {
            //     current_state = ROAMING;
            // }
            break;
        }

        case PURSUE_WEIGHT:
            state = weightDetection();
            if (state.direction == CENTER) {
                if (state.direction < 400) {
                    current_state = COLLECT_WEIGHT;
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

        case RETURNING_BASE: {
            if (state.certainty > 1) {
                // Use IR camera to follow beacon home
            }
            break;
        }
    }

    // Debugging output
    Serial2.print("Current state: ");
    switch (current_state) {
        case ROAMING: Serial2.println("ROAMING"); break;
        case COLLECT_WEIGHT: Serial2.println("COLLECTING_WEIGHT"); break;
        case RETURNING_BASE: Serial2.println("RETURNING_BASE"); break;
        case RANDOM_WALK: Serial2.println("RANDOM_WALK"); break;
        case PURSUE_WEIGHT: Serial2.println("PURSUE_WEIGHT"); break;
    }
}
