#include "StateMachine.h"
#include "TOFControl.h"
#include "WeightDetection.h"
#include "Movement.h"

typedef enum {
  ROAMING,
  WEIGHT_FINDING,
  COLLECT_WEIGHT,
} Robot_states;

static Robot_states current_state = ROAMING;

void Robot_State_Machine() {
    weight_info_t state;
    switch (current_state) {
        case ROAMING:
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
            } else {
                MoveMent_Controller();
            }
            break;

        case WEIGHT_FINDING:
            break;

        case COLLECT_WEIGHT:
            if (state.certainty > 1) {
                SlowForward();
            } else {
                current_state = ROAMING;
            }
            break;
    }

    Serial2.print("Current state: ");
    switch (current_state) {
        case ROAMING: Serial2.println("ROAMING"); break;
        case WEIGHT_FINDING: Serial2.println("WEIGHT_FINDING"); break;
        case COLLECT_WEIGHT: Serial2.println("COLLECTING_WEIGHT"); break;
    }
}
