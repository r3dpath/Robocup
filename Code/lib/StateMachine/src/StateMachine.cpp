#include "StateMachine.h"
#include "TOFControl.h"
#include "WeightDetection.h"
#include "Movement.h"

typedef enum {
  ROAMING,
  WEIGHT_FINDING,
  COLLECT_WEIGHT,
  RETURNING_BASE
} Robot_states;

static Robot_states current_state = ROAMING;
unsigned long startTime;
uint8_t num_weight = 0;

void setup() {
    startTime = millis(); //initialize the timer when the robot start running
}

void Robot_State_Machine() {

    weight_info_t state;
    unsigned long elapsedTime = millis() - startTime; //Robot run time

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
                //stops the robot if timer is over 2 minutes
            } else if (elapsedTime > 120000) { 
                Stationary();
            } else {
                MoveMent_Controller();
            } 

            break;

        case WEIGHT_FINDING:
            break;

        case COLLECT_WEIGHT:
            if (state.certainty > 1) {
                SlowForward();
                //we should have it so once weight is detected it triggers something here LIM SW? or the IR Sensor
                num_weight += 1; //just a dummy code to check state switch
            } else if ((num_weight == 3) && (elapsedTime < 105000) ) { //check if the run time is less than 1 minute 45 seconds otherwise dont bother going home
                current_state = RETURNING_BASE;
            } else {
                current_state = ROAMING;
            break;

        case RETURNING_BASE:
            if (state.certainty > 1) {
                //use IR camera to follow beacon home
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
}
