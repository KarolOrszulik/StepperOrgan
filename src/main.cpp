#include <Arduino.h>

#include "Stepper.h"
#include "stepper_pins.h"
#include "MidiFrequency.h"
#include "MidiParser.h"

static byte stepperNote[numSteppers()] {};
static Stepper steppers[numSteppers()];

static MidiParser midiParser;

int findStepperWithNote(byte note)
{
    for (int i = 0; i < numSteppers(); i++)
    {
        if (stepperNote[i] == note)
            return i;
    }

    return -1;
}

void onNotePressed(byte note, byte velocity, byte channel)
{
    digitalWrite(LED_BUILTIN, HIGH);

    int freeStepperIdx = findStepperWithNote(0);
    if (freeStepperIdx == -1)
        return;
    
    stepperNote[freeStepperIdx] = note;
    steppers[freeStepperIdx].playNote(note);
}

void onNoteReleased(byte note, byte velocity, byte channel)
{
    digitalWrite(LED_BUILTIN, LOW);

    int stepperIdx = findStepperWithNote(note);
    if (stepperIdx == -1)
        return;

    stepperNote[stepperIdx] = 0;
    steppers[stepperIdx].stop();
}

void onAllOff()
{
    digitalWrite(LED_BUILTIN, LOW);

    for (int i = 0; i < numSteppers(); i++)
    {
        stepperNote[i] = 0;
        steppers[i].stop();
    }
}

void IRAM_ATTR updateSteppers()
{
    for (int i = 0; i < numSteppers(); i++)
        steppers[i].update();
}

void createUpdateInterrupt()
{
    hw_timer_t* timer = timerBegin(0, 80, true);
    const uint64_t updatePeriod = 1e6 / 50000;
    timerAttachInterrupt(timer, &updateSteppers, true);
    timerAlarmWrite(timer, updatePeriod, true);
    timerAlarmEnable(timer);
}

void setup()
{
    Serial.begin(38400);
    Serial1.begin(38400, SERIAL_8N1, 16, 17);

    MidiFrequency::generateFrequencyTable(1175.f);

    midiParser.setNotePressedCallback(onNotePressed);
    midiParser.setNoteReleasedCallback(onNoteReleased);
    midiParser.setAllOffCallback(onAllOff);


    for (int i = 0; i < numSteppers(); i++)
    {
        steppers[i].setPins(STEPPER_PINS[i].step, STEPPER_PINS[i].en);
        steppers[i].stop();
    }

    createUpdateInterrupt();

    pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
    midiParser.read();
}
 