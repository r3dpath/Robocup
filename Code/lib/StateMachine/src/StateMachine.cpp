#include "StateMachine.h"
#include "TOFControl.h"
#include "WeightDetection.h"
#include "Movement.h"

typedef enum {
  ROAMING,
  WEIGHT_FINDING,
  COLLECTING_WEIGHT,
} Robot_states;

static Robot_states current_state = ROAMING;

void Robot_State_Machine() {
    switch (current_state) {
        case ROAMING:
            MoveMent_Controller();
            if (getFindWeightFlag()) {
                current_state = WEIGHT_FINDING;
            }
            break;

        case WEIGHT_FINDING:
            MoveMent_Controller();
            if (getFoundWeightFlag()) {
                current_state = COLLECTING_WEIGHT;
            } else if (!getFindWeightFlag()) {
                current_state = ROAMING;
            }
            break;

        case COLLECTING_WEIGHT:
            delay(2000);
            current_state = ROAMING;
            break;
    }

    Serial2.print("Current state: ");
    switch (current_state) {
        case ROAMING: Serial2.println("ROAMING"); break;
        case WEIGHT_FINDING: Serial2.println("WEIGHT_FINDING"); break;
        case COLLECTING_WEIGHT: Serial2.println("COLLECTING_WEIGHT"); break;
    }
}
