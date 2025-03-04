#pragma once

#include <Arduino.h>

class MidiFrequency
{
public:
    static float freq(byte midiNoteNum);
    static void generateFrequencyTable(float maxFreq = 20e3);

private:
    static float frequencyTable[128];
};