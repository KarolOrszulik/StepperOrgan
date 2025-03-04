#pragma once

#include <Arduino.h>

class MidiParser
{
public:
    using NoteCallback = void (*)(byte note, byte velocity, byte channel);
    using AllOfCallback = void (*)();

    void read();
    
    void setNotePressedCallback(NoteCallback callback);
    void setNoteReleasedCallback(NoteCallback callback);
    void setAllOffCallback(AllOfCallback callback);

private:
    bool isStatusByte(byte inByte);
    void parseMidiMessage();

    NoteCallback notePressedCallback = nullptr;
    NoteCallback noteReleasedCallback = nullptr;
    AllOfCallback allOffCallback = nullptr;

    byte lastStatusByte = 0;
    byte dataBytes[2] = {0, 0};
    byte dataBytesIdx = 0;
};