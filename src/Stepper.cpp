#include <Arduino.h>

#include "Stepper.h"
#include "MidiFrequency.h"

void Stepper::setPins(byte step, byte en)
{
    stepPin = step;
    enPin = en;

    pinMode(stepPin, OUTPUT);
    pinMode(enPin, OUTPUT);
    digitalWrite(enPin, HIGH);
}

void Stepper::stop()
{
    digitalWrite(enPin, HIGH);
    stepInterval = UINT32_MAX;
}

void Stepper::playNote(byte midiNote)
{
    digitalWrite(enPin, LOW);
    stepInterval = 1e6 / MidiFrequency::freq(midiNote);
    lastStepTime = micros();
}

void Stepper::update()
{
    unsigned long currentTime = micros();
    unsigned long timeSinceLastStep = currentTime - lastStepTime;

    if (timeSinceLastStep >= stepInterval)
    {
        digitalWrite(stepPin, !digitalRead(stepPin));
        lastStepTime = currentTime;
    }
}