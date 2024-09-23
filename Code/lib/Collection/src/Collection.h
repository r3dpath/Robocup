#ifndef COLLECTION_H
#define COLLECTION_H

#include <PulsePosition.h>

#define PIN_STEPA 2
#define PIN_DIRA 3
#define PIN_STEPB 4
#define PIN_DIRB 5

void initCollection();
void collectionOn();
void collectionReverse();
void collectionOff();

#endif // COLLECTION_H