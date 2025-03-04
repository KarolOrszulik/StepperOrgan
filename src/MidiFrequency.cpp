#include "MidiFrequency.h"

#include <Arduino.h>

float MidiFrequency::frequencyTable[128] = {};

float MidiFrequency::freq(byte midiNoteNum)
{
    return frequencyTable[midiNoteNum];
}

void MidiFrequency::generateFrequencyTable(float maxFreq)
{
    for (int i = 0; i < 128; i++)
    {
        frequencyTable[i] = 440.0 * pow(2.0, (i - 69) / 12.0);
        if (frequencyTable[i] > maxFreq)
        {
            // assert(i >= 12);
            frequencyTable[i] = frequencyTable[i-12];
        }
    }
}
