#ifndef STATEMACHINE_H
#define STATEMACHINE_H

typedef enum {
  ROAMING,
  COLLECT_WEIGHT,
  RETURNING_BASE,
  RANDOM_WALK,
  PURSUE_WEIGHT
} Robot_states;

void Robot_State_Machine();
void transition(Robot_states newState);

#endif
