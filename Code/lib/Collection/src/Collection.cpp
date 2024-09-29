#include "Collection.h"
#include "debug.h"

void initCollection() {
    pinMode(PIN_STEPA, OUTPUT);
    pinMode(PIN_DIRA, OUTPUT);
    pinMode(PIN_STEPB, OUTPUT);
    pinMode(PIN_DIRB, OUTPUT);

    analogWriteFrequency(PIN_STEPA, 1000);
    analogWriteFrequency(PIN_STEPB, 1000);
}

void collectionOn() {
    digitalWrite(PIN_DIRA, HIGH);
    digitalWrite(PIN_DIRB, HIGH);
    analogWrite(PIN_STEPA, 5);
    analogWrite(PIN_STEPB, 5);
}

void collectionReverse() {
    digitalWrite(PIN_DIRA, LOW);
    digitalWrite(PIN_DIRB, LOW);
    analogWrite(PIN_STEPA, 5);
    analogWrite(PIN_STEPB, 5);
}

void collectionOff() {
    digitalWrite(PIN_DIRA, LOW);
    digitalWrite(PIN_DIRB, LOW);
    analogWrite(PIN_STEPA, 0);
    analogWrite(PIN_STEPB, 0);
}