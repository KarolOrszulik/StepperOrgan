#include "MidiParser.h"

void MidiParser::read()
{
    if (!Serial.available())
        return;

    byte inByte = Serial.read();

    if (isStatusByte(inByte))
    {
        lastStatusByte = inByte;
        dataBytesIdx = 0;
    }
    else
    {
        dataBytes[dataBytesIdx++] = inByte;

        if (dataBytesIdx == 2)
            parseMidiMessage();
    }
}

bool MidiParser::isStatusByte(byte inByte)
{
    return inByte & 0x80;
}

void MidiParser::parseMidiMessage()
{
    constexpr byte NOTE_ON  = 0x90;
    constexpr byte NOTE_OFF = 0x80;
    constexpr byte ALL_NOTES_OFF = 0xB3;
    constexpr byte ALL_SOUND_OFF = 0xB0;

    const byte maskedCommand = lastStatusByte & 0xF0;
    const byte channel = (lastStatusByte & 0x0F) + 1;

    if (maskedCommand == NOTE_OFF || (maskedCommand == NOTE_ON && dataBytes[1] == 0))
        noteReleasedCallback(dataBytes[0], dataBytes[1], channel);
    else if (maskedCommand == NOTE_ON)
        notePressedCallback(dataBytes[0], dataBytes[1], channel);
    else if (lastStatusByte == ALL_NOTES_OFF || lastStatusByte == ALL_SOUND_OFF)
        allOffCallback();

    dataBytesIdx = 0; // to handle running status
}

void MidiParser::setNotePressedCallback(NoteCallback callback)
{
    notePressedCallback = callback;
}

void MidiParser::setNoteReleasedCallback(NoteCallback callback)
{
    noteReleasedCallback = callback;
}

void MidiParser::setAllOffCallback(AllOfCallback callback)
{
    allOffCallback = callback;
}