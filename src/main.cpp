#include <Arduino.h>

#include "Stepper.h"
#include "MidiFrequency.h"
#include "stepper_pins.h"

static byte stepperNote[numSteppers()] {};
static Stepper steppers[numSteppers()];


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

void readAvailableMIDI()
{
    if (Serial.available() < 3)
        return;

    constexpr byte ANY_CHANNEL_NOTE_ON  = 0x90;
    constexpr byte ANY_CHANNEL_NOTE_OFF = 0x80;

    const byte command = Serial.read();
    const byte note = Serial.read();
    const byte velocity = Serial.read();

    const byte maskedCommand = command & 0xF0;
    const byte channel = (command & 0x0F) + 1;

    if (maskedCommand == ANY_CHANNEL_NOTE_OFF || (maskedCommand == ANY_CHANNEL_NOTE_ON && velocity == 0))
        onNoteReleased(note, velocity, channel);
    else if (maskedCommand == ANY_CHANNEL_NOTE_ON)
        onNotePressed(note, velocity, channel);
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
    readAvailableMIDI();
}
 