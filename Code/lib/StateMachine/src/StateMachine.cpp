#include "StateMachine.h"
#include "TOFControl.h"
#include "WeightDetection.h"
#include "Movement.h"

typedef enum {
  ROAMING,
  COLLECT_WEIGHT,
  RETURNING_BASE,
  RANDOM_WALK
} Robot_states;

static Robot_states current_state = ROAMING;
unsigned long startTime;
uint8_t num_weight = 0;
unsigned long lastTurnTime = 0;

void setup_timer() {
    startTime = millis(); // Initialize the timer when the robot starts running
}

void Larger_Left_Turn() {
    LeftTurn();
    delay(500); // Delay for large turn
}

void Larger_Right_Turn() {
    RightTurn();
    delay(500); // Delay for large turn
}

void Robot_State_Machine() {
    weight_info_t state;
    unsigned long elapsedTime = millis() - startTime; // Robot run time

    switch (current_state) {
        case ROAMING: {
            state = weightDetection();
            if (state.certainty > 1) {
                if (state.direction == CENTER) {
                    current_state = COLLECT_WEIGHT;
                } else {
                    if (state.direction == LEFT || state.direction == FAR_LEFT) {
                        smallLeft();
                    } else {
                        smallRight();
                    }
                }
            }
            // Stops the robot if timer is over 2 minutes
            else if (elapsedTime > 120000) { 
                Stationary();
            } else {
                MoveMent_Controller();
                if (millis() - lastTurnTime > (unsigned long)random(2000, 5000)) {
                    current_state = RANDOM_WALK;
                    lastTurnTime = millis();  // Reset the timer for random walk duration
                }
            }
            break;
        }

        case RANDOM_WALK: {
            // Randomly pick direction (0 for left, 1 for right) and turn type (0 for small, 1 for large)
            int randomTurn = random(0, 2);
            int turnType = random(0, 2);

            if (randomTurn == 0) {
                if (turnType == 0) {
                    smallLeft();
                } else {
                    Larger_Left_Turn();
                }
            } else {
                if (turnType == 0) {
                    smallRight();
                } else {
                    Larger_Right_Turn();
                }
            }

            // Stay in random walk mode for 2-5 seconds
            if (millis() - lastTurnTime > (unsigned long)random(2000, 5000)) {
                current_state = ROAMING;
                lastTurnTime = millis();  // Reset timer for roaming
            }
            break;
        }

        case COLLECT_WEIGHT: {
            
            SlowForward();
            num_weight += 1; // Increment weight count

            if ((num_weight == 3) && (elapsedTime < 105000)) { // Return home if 3 weights collected and within time
                current_state = RETURNING_BASE;
            } else {
                current_state = ROAMING;
            }
            break;
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
    }
}
