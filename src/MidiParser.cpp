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
    Serial1.printf("%02X %02X %02X\n", lastStatusByte, dataBytes[0], dataBytes[1]);

    constexpr byte NOTE_ON  = 0x90;
    constexpr byte NOTE_OFF = 0x80;
    constexpr byte CONTROL_CHANGE = 0xB0;

    const byte statusType = lastStatusByte & 0xF0;
    const byte channel = (lastStatusByte & 0x0F) + 1;

    switch (statusType)
    {
    case NOTE_ON:
        if (dataBytes[1] != 0)
        {
            notePressedCallback(dataBytes[0], dataBytes[1], channel);
            break;
        }
        // else fall through to NOTE_OFF

    case NOTE_OFF:
        noteReleasedCallback(dataBytes[0], dataBytes[1], channel);
        break;

    case CONTROL_CHANGE:
        if (dataBytes[0] == 123 || dataBytes[0] == 120)
            allOffCallback();
        break;

    default:
        break; // ignore unimplemented MIDI messages
    }

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